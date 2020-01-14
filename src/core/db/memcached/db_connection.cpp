/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

    This file is part of FastoNoSQL.

    FastoNoSQL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoNoSQL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoNoSQL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fastonosql/core/db/memcached/db_connection.h>

#include <libmemcached/memcached.h>
#include <libmemcached/util.h>

#include <fastonosql/core/db/memcached/command_translator.h>
#include <fastonosql/core/db/memcached/config.h>
#include <fastonosql/core/db/memcached/database_info.h>
#include "core/db/memcached/internal/commands_api.h"

// hacked
struct hacked_memcached_instance_st {
  struct {
    bool is_allocated;
    bool is_initialized;
    bool is_shutting_down;
    bool is_dead;
    bool ready;
  } options;

  short _events;
  short _revents;

  uint32_t cursor_active_;
  in_port_t port_;
  memcached_socket_t fd;
  uint32_t io_bytes_sent; /* # bytes sent since last read */
  uint32_t request_id;
  uint32_t server_failure_counter;
  uint64_t server_failure_counter_query_id;
  uint32_t server_timeout_counter;
  uint32_t server_timeout_counter_query_id;
  uint32_t weight;
  uint32_t version;
  enum memcached_server_state_t state;
  struct {
    uint32_t read;
    uint32_t write;
    uint32_t timeouts;
    size_t _bytes_read;
  } io_wait_count;
  uint8_t major_version;  // Default definition of UINT8_MAX means that it has
                          // not been set.
  uint8_t micro_version;  // ditto, and note that this is the third, not second
                          // version bit
  uint8_t minor_version;  // ditto
  memcached_connection_t type;
  char* read_ptr;
  size_t read_buffer_length;
  size_t read_data_length;
  size_t write_buffer_offset;
  struct addrinfo* address_info;
  struct addrinfo* address_info_next;
  time_t next_retry;
  struct memcached_st* root;
  uint64_t limit_maxbytes;
  struct memcached_error_t* error_messages;
  char read_buffer[MEMCACHED_MAX_BUFFER];
  char write_buffer[MEMCACHED_MAX_BUFFER];
  char _hostname[MEMCACHED_NI_MAXHOST];
};

namespace {

struct KeysHolder {
  const fastonosql::core::raw_key_t key_start;
  const fastonosql::core::raw_key_t key_end;
  const fastonosql::core::keys_limit_t limit;
  std::vector<fastonosql::core::raw_key_t> result;

  KeysHolder(const fastonosql::core::raw_key_t& key_start,
             const fastonosql::core::raw_key_t& key_end,
             fastonosql::core::keys_limit_t limit)
      : key_start(key_start), key_end(key_end), limit(limit), result() {}

  memcached_return_t AddKey(const char* key, size_t key_length, time_t exp) {
    UNUSED(exp);
    if (result.size() < limit) {
      const fastonosql::core::raw_key_t received_key = GEN_CMD_STRING_SIZE(key, key_length);
      if (fastonosql::core::IsKeyInRange(key_start, received_key, key_end)) {
        result.push_back(received_key);
        return MEMCACHED_SUCCESS;
      }

      return MEMCACHED_SUCCESS;
    }

    return MEMCACHED_END;
  }
};

memcached_return_t memcached_dump_keys_callback(const memcached_st* ptr,
                                                const char* key,
                                                size_t key_length,
                                                time_t exp,
                                                void* context) {
  UNUSED(ptr);

  KeysHolder* holder = static_cast<KeysHolder*>(context);
  return holder->AddKey(key, key_length, exp);
}

struct ScanHolder {
  ScanHolder(fastonosql::core::cursor_t cursor_in, const std::string& pattern, fastonosql::core::keys_limit_t limit)
      : cursor_in(cursor_in), limit(limit), pattern(pattern), cursor_out(0), offset_pos(cursor_in), result() {}

  const fastonosql::core::cursor_t cursor_in;
  const fastonosql::core::keys_limit_t limit;
  const std::string pattern;
  fastonosql::core::cursor_t cursor_out;
  fastonosql::core::cursor_t offset_pos;
  std::vector<fastonosql::core::command_buffer_t> result;

  memcached_return_t AddKey(const char* key, size_t key_length, time_t exp) {
    UNUSED(exp);
    if (result.size() < limit) {
      if (fastonosql::core::IsKeyMatchPattern(key, key_length, pattern)) {
        if (offset_pos == 0) {
          result.push_back(GEN_CMD_STRING_SIZE(key, key_length));
        } else {
          offset_pos--;
        }
      }
      return MEMCACHED_SUCCESS;
    } else {
      cursor_out = cursor_in + limit;
    }

    return MEMCACHED_END;
  }
};

memcached_return_t memcached_dump_scan_callback(const memcached_st* ptr,
                                                const char* key,
                                                size_t key_length,
                                                time_t exp,
                                                void* context) {
  UNUSED(ptr);

  ScanHolder* holder = static_cast<ScanHolder*>(context);
  return holder->AddKey(key, key_length, exp);
}

struct TTLHolder {
  TTLHolder(const fastonosql::core::readable_string_t& key, time_t* exp) : looked_key(key), exp_out(exp) {}
  memcached_return_t CheckKey(const char* key, size_t key_length, time_t exp) {
    fastonosql::core::readable_string_t received_key = GEN_CMD_STRING_SIZE(key, key_length);
    if (received_key == looked_key) {
      *exp_out = exp;
      return MEMCACHED_END;
    }

    return MEMCACHED_SUCCESS;
  }

  const fastonosql::core::readable_string_t looked_key;
  time_t* exp_out;
};

memcached_return_t memcached_dump_ttl_callback(const memcached_st* ptr,
                                               const char* key,
                                               size_t key_length,
                                               time_t exp,
                                               void* context) {
  UNUSED(ptr);

  TTLHolder* holder = static_cast<TTLHolder*>(context);
  return holder->CheckKey(key, key_length, exp);
}

}  // namespace

namespace fastonosql {
namespace core {
namespace memcached {
namespace {
const ConstantCommandsArray kCommands = {CommandHolder(GEN_CMD_STRING(DB_HELP_COMMAND),
                                                       "[command]",
                                                       "Return how to use command",
                                                       UNDEFINED_SINCE,
                                                       DB_HELP_COMMAND " " DB_GET_KEY_COMMAND,
                                                       0,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Help),
                                         CommandHolder(GEN_CMD_STRING(DB_INFO_COMMAND),
                                                       "[section]",
                                                       "These command return database information.",
                                                       UNDEFINED_SINCE,
                                                       DB_INFO_COMMAND " STATS",
                                                       0,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Info),
                                         CommandHolder(GEN_CMD_STRING(DB_GET_CONFIG_COMMAND),
                                                       "<parameter>",
                                                       "Get the value of a configuration parameter",
                                                       UNDEFINED_SINCE,
                                                       DB_GET_CONFIG_COMMAND " databases",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::ConfigGet),
                                         CommandHolder(GEN_CMD_STRING("INCR"),
                                                       "<key> <value>",
                                                       "Increment value associated with key in "
                                                       "Memcached, item must "
                                                       "exist, increment "
                                                       "command will not create it.\n"
                                                       "The limit of increment is the 64 bit mark.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Incr),
                                         CommandHolder(GEN_CMD_STRING("DECR"),
                                                       "<key> <value>",
                                                       "Decrement value associated with key "
                                                       "in Memcached, item must "
                                                       "exist, decrement "
                                                       "command will not create it.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Decr),
                                         CommandHolder(GEN_CMD_STRING("PREPEND"),
                                                       "<key> <flags> <exptime> <bytes>",
                                                       "Add value to an existing key before "
                                                       "existing data.\n"
                                                       "Prepend does not take <flags> or "
                                                       "<exptime> parameters but "
                                                       "you must provide them!",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Prepend),
                                         CommandHolder(GEN_CMD_STRING("APPEND"),
                                                       "<key> <flags> <exptime> <value>",
                                                       "Add value to an existing key after "
                                                       "existing data.\n"
                                                       "Append does not take <flags> or "
                                                       "<exptime> parameters but "
                                                       "you must provide them!",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Append),
                                         CommandHolder(GEN_CMD_STRING("REPLACE"),
                                                       "<key> <flags> <exptime> <value>",
                                                       "Store key/value pair in Memcached, "
                                                       "but only if the server "
                                                       "already hold data for this key.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Replace),
                                         CommandHolder(GEN_CMD_STRING("ADD"),
                                                       "<key> <flags> <exptime> <value>",
                                                       "Store key/value pair in Memcached, "
                                                       "but only if the server "
                                                       "doesn't already hold "
                                                       "data for this key.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Add),
                                         CommandHolder(GEN_CMD_STRING(DB_SCAN_COMMAND),
                                                       "<cursor> [MATCH pattern] [COUNT count]",
                                                       "Incrementally iterate the keys space",
                                                       UNDEFINED_SINCE,
                                                       DB_SCAN_COMMAND " 0 MATCH * COUNT 10",
                                                       1,
                                                       4,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Scan),
                                         CommandHolder(GEN_CMD_STRING(DB_JSONDUMP_COMMAND),
                                                       "<cursor> PATH <absolute_path> [MATCH pattern] [COUNT count]",
                                                       "Dump DB into json file by path.",
                                                       UNDEFINED_SINCE,
                                                       DB_JSONDUMP_COMMAND " 0 PATH ~/dump.json MATCH * COUNT 10",
                                                       3,
                                                       4,
                                                       CommandInfo::Native,
                                                       &CommandsApi::JsonDump),
                                         CommandHolder(GEN_CMD_STRING(DB_CSVDUMP_COMMAND),
                                                       "<cursor> PATH <absolute_path> [MATCH pattern] [COUNT count]",
                                                       "Dump DB into csv file by path.",
                                                       UNDEFINED_SINCE,
                                                       DB_CSVDUMP_COMMAND " 0 PATH ~/dump.csv MATCH * COUNT 10",
                                                       3,
                                                       4,
                                                       CommandInfo::Native,
                                                       &CommandsApi::CsvDump),
                                         CommandHolder(GEN_CMD_STRING(DB_STORE_VALUE_COMMAND),
                                                       "<key> PATH <absolute_path>",
                                                       "Save value to file by path.",
                                                       UNDEFINED_SINCE,
                                                       DB_STORE_VALUE_COMMAND " key PATH ~/key_raw",
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::StoreValue),
                                         CommandHolder(GEN_CMD_STRING(DB_KEY_TYPE_COMMAND),
                                                       "<key>",
                                                       "Determine the type stored at key",
                                                       UNDEFINED_SINCE,
                                                       DB_KEY_TYPE_COMMAND " key",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::GetType),
                                         CommandHolder(GEN_CMD_STRING(DB_KEYS_COMMAND),
                                                       "<key_start> <key_end> <limit>",
                                                       "Find all keys matching the given limits.",
                                                       UNDEFINED_SINCE,
                                                       DB_KEYS_COMMAND " a z 10",
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Keys),
                                         CommandHolder(GEN_CMD_STRING(DB_DBKCOUNT_COMMAND),
                                                       "-",
                                                       "Return the number of keys in the "
                                                       "selected database",
                                                       UNDEFINED_SINCE,
                                                       DB_DBKCOUNT_COMMAND,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::DBKeysCount),
                                         CommandHolder(GEN_CMD_STRING(DB_FLUSHDB_COMMAND),
                                                       "-",
                                                       "Remove all keys from the current database",
                                                       UNDEFINED_SINCE,
                                                       DB_FLUSHDB_COMMAND,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::FlushDB),
                                         CommandHolder(GEN_CMD_STRING(DB_SELECTDB_COMMAND),
                                                       "<name>",
                                                       "Change the selected database for the "
                                                       "current connection",
                                                       UNDEFINED_SINCE,
                                                       DB_SELECTDB_COMMAND " 0",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Select),
                                         CommandHolder(GEN_CMD_STRING(DB_SET_KEY_COMMAND),
                                                       "<key> <value>",
                                                       "Set the value of a key.",
                                                       UNDEFINED_SINCE,
                                                       DB_SET_KEY_COMMAND " key value",
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Set),
                                         CommandHolder(GEN_CMD_STRING(DB_GET_KEY_COMMAND),
                                                       "<key>",
                                                       "Get the value of a key.",
                                                       UNDEFINED_SINCE,
                                                       DB_GET_KEY_COMMAND " key",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Get),
                                         CommandHolder(GEN_CMD_STRING(DB_GETUNI_KEY_COMMAND),
                                                       "<key>",
                                                       "Get the value of a key.",
                                                       UNDEFINED_SINCE,
                                                       DB_GETUNI_KEY_COMMAND " key",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::GetUni),
                                         CommandHolder(GEN_CMD_STRING(DB_RENAME_KEY_COMMAND),
                                                       "<key> <newkey>",
                                                       "Rename a key",
                                                       UNDEFINED_SINCE,
                                                       DB_RENAME_KEY_COMMAND " old_name new_name",
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Rename),
                                         CommandHolder(GEN_CMD_STRING(DB_DELETE_KEY_COMMAND),
                                                       "<key> [key ...]",
                                                       "Delete key.",
                                                       UNDEFINED_SINCE,
                                                       DB_DELETE_KEY_COMMAND " key",
                                                       1,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Delete),
                                         CommandHolder(GEN_CMD_STRING(DB_SET_TTL_COMMAND),
                                                       "<key> <exptime>",
                                                       "Set a key's time to live in seconds",
                                                       UNDEFINED_SINCE,
                                                       DB_SET_TTL_COMMAND " key 10",
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::SetTTL),
                                         CommandHolder(GEN_CMD_STRING(DB_GET_TTL_COMMAND),
                                                       "<key>",
                                                       "Get the time to live for a key",
                                                       UNDEFINED_SINCE,
                                                       DB_GET_TTL_COMMAND " key",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::GetTTL),
                                         CommandHolder(GEN_CMD_STRING(DB_QUIT_COMMAND),
                                                       "-",
                                                       "Close the connection",
                                                       UNDEFINED_SINCE,
                                                       DB_QUIT_COMMAND,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Quit)};
}  // namespace
}  // namespace memcached
template <>
const char* ConnectionTraits<MEMCACHED>::GetBasedOn() {
  return "libmemcached";
}

template <>
const char* ConnectionTraits<MEMCACHED>::GetVersionApi() {
  return memcached_lib_version();
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<MEMCACHED>::GetCommands() {
  return memcached::kCommands;
}
namespace internal {
template <>
common::Error Connection<memcached::NativeConnection, memcached::Config>::Connect(const memcached::Config& config,
                                                                                  memcached::NativeConnection** hout) {
  memcached::NativeConnection* context = nullptr;
  common::Error err = memcached::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error Connection<memcached::NativeConnection, memcached::Config>::Disconnect(
    memcached::NativeConnection** handle) {
  memcached::NativeConnection* lhandle = *handle;
  if (lhandle) {
    memcached_free(lhandle);
  }
  lhandle = nullptr;
  return common::Error();
}

template <>
bool Connection<memcached::NativeConnection, memcached::Config>::IsConnected(memcached::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  hacked_memcached_instance_st* servers = reinterpret_cast<hacked_memcached_instance_st*>(handle->servers);
  if (!servers) {
    return false;
  }

  return servers->state == MEMCACHED_SERVER_STATE_CONNECTED;
}

}  // namespace internal
namespace memcached {

common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  memcached_st* memc = ::memcached(nullptr, 0);
  if (!memc) {
    return common::make_error("Init error");
  }

  memcached_return rc;
  if (!config.user.empty() && !config.password.empty()) {
    const char* user = config.user.c_str();
    const char* passwd = config.password.c_str();
    rc = memcached_set_sasl_auth_data(memc, user, passwd);
    if (rc != MEMCACHED_SUCCESS) {
      memcached_free(memc);
      return common::make_error(common::MemSPrintf("Couldn't setup SASL auth: %s", memcached_strerror(memc, rc)));
    }
  }

  std::string host_str = config.host.GetHost();
  const char* host = host_str.empty() ? nullptr : host_str.c_str();
  uint16_t hostport = config.host.GetPort();

  rc = memcached_server_add(memc, host, hostport);

  if (rc != MEMCACHED_SUCCESS) {
    memcached_free(memc);
    return common::make_error(common::MemSPrintf("Couldn't add server: %s", memcached_strerror(memc, rc)));
  }

  memcached_return_t error = memcached_version(memc);
  if (error != MEMCACHED_SUCCESS) {
    memcached_free(memc);
    return common::make_error(common::MemSPrintf("Connect to server error: %s", memcached_strerror(memc, error)));
  }

  *context = memc;
  return common::Error();
}

common::Error TestConnection(const Config& config) {
  std::string host_str = config.host.GetHost();
  const char* host = host_str.empty() ? nullptr : host_str.c_str();
  uint16_t hostport = config.host.GetPort();

  memcached_return rc;
  if (!config.user.empty() && !config.password.empty()) {
    const char* user = config.user.c_str();
    const char* passwd = config.password.c_str();
    libmemcached_util_ping2(host, hostport, user, passwd, &rc);
    if (rc != MEMCACHED_SUCCESS) {
      return common::make_error(common::MemSPrintf("Couldn't ping server: %s", memcached_strerror(nullptr, rc)));
    }
  } else {
    libmemcached_util_ping(host, hostport, &rc);
    if (rc != MEMCACHED_SUCCESS) {
      return common::make_error(common::MemSPrintf("Couldn't ping server: %s", memcached_strerror(nullptr, rc)));
    }
  }

  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())), current_info_() {}

common::Error DBConnection::Info(const std::string& args, ServerInfo::Stats* statsout) {
  if (!statsout) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const char* stabled_args = nullptr;
  if (!args.empty()) {
    stabled_args = args.c_str();
  }

  memcached_return_t error;
  memcached_stat_st* st = memcached_stat(connection_.handle_, const_cast<char*>(stabled_args), &error);
  err = CheckResultCommand(DB_INFO_COMMAND, error);
  if (err) {
    return err;
  }

  ServerInfo::Stats lstatsout;
  lstatsout.pid = st->pid;
  lstatsout.uptime = st->uptime;
  lstatsout.time = st->time;
  lstatsout.version = st->version;
  lstatsout.pointer_size = st->pointer_size;
  lstatsout.rusage_user = st->rusage_user_seconds;
  lstatsout.rusage_system = st->rusage_system_seconds;
  lstatsout.curr_items = st->curr_items;
  lstatsout.total_items = st->total_items;
  lstatsout.bytes = st->bytes;
  lstatsout.curr_connections = st->curr_connections;
  lstatsout.total_connections = st->total_connections;
  lstatsout.connection_structures = st->connection_structures;
  lstatsout.cmd_get = st->cmd_get;
  lstatsout.cmd_set = st->cmd_set;
  lstatsout.get_hits = st->get_hits;
  lstatsout.get_misses = st->get_misses;
  lstatsout.evictions = st->evictions;
  lstatsout.bytes_read = st->bytes_read;
  lstatsout.bytes_written = st->bytes_written;
  lstatsout.limit_maxbytes = st->limit_maxbytes;
  lstatsout.threads = st->threads;

  *statsout = lstatsout;
  current_info_ = lstatsout;
  memcached_stat_free(nullptr, st);
  return common::Error();
}

common::Error DBConnection::AddIfNotExist(const NKey& key,
                                          const raw_value_t& value,
                                          time_t expiration,
                                          uint32_t flags) {
  if (value.empty()) {
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();
  const auto key_slice = key_str.GetData();
  err = CheckResultCommand("ADD", memcached_add(connection_.handle_, key_slice.data(), key_slice.size(), value.data(),
                                                value.size(), expiration, flags));
  if (err) {
    return err;
  }

  if (client_) {
    NDbKValue dbv(key, NValue(common::Value::CreateStringValue(value)));
    client_->OnAddedKey(dbv);
  }
  return common::Error();
}

common::Error DBConnection::Replace(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags) {
  if (value.empty()) {
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();
  const auto key_slice = key_str.GetData();
  err = CheckResultCommand("REPLACE", memcached_replace(connection_.handle_, key_slice.data(), key_slice.size(),
                                                        value.data(), value.size(), expiration, flags));
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnLoadedKey(NDbKValue(key, NValue(common::Value::CreateStringValue(value))));
  }
  return common::Error();
}

common::Error DBConnection::Append(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags) {
  if (value.empty()) {
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();
  const auto key_slice = key_str.GetData();
  err = CheckResultCommand("APPEND", memcached_append(connection_.handle_, key_slice.data(), key_slice.size(),
                                                      value.data(), value.size(), expiration, flags));
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnAddedKey(NDbKValue(key, NValue(common::Value::CreateStringValue(value))));
  }
  return common::Error();
}

common::Error DBConnection::Prepend(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags) {
  if (value.empty()) {
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();
  const auto key_slice = key_str.GetData();
  err = CheckResultCommand("PREPEND", memcached_prepend(connection_.handle_, key_slice.data(), key_slice.size(),
                                                        value.data(), value.size(), expiration, flags));
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnAddedKey(NDbKValue(key, NValue(common::Value::CreateStringValue(value))));
  }
  return common::Error();
}

common::Error DBConnection::Incr(const NKey& key, uint32_t value, uint64_t* result) {
  if (!result) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();
  const auto key_slice = key_str.GetData();
  uint64_t local_value = 0;
  err = CheckResultCommand(
      "INCR", memcached_increment(connection_.handle_, key_slice.data(), key_slice.size(), value, &local_value));
  if (err) {
    return err;
  }

  if (client_) {
    auto result_str = common::ConvertToCharBytes(local_value);
    NValue val(common::Value::CreateStringValue(result_str));  // type of key should be string
    client_->OnAddedKey(NDbKValue(key, val));
  }
  *result = local_value;
  return common::Error();
}

common::Error DBConnection::Decr(const NKey& key, uint32_t value, uint64_t* result) {
  if (!result) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();
  const auto key_slice = key_str.GetData();
  uint64_t local_value = 0;
  err = CheckResultCommand(
      "DECR", memcached_decrement(connection_.handle_, key_slice.data(), key_slice.size(), value, &local_value));
  if (err) {
    return err;
  }

  if (client_) {
    auto result_str = common::ConvertToCharBytes(local_value);
    NValue val(common::Value::CreateStringValue(result_str));  // type of key should be string
    client_->OnAddedKey(NDbKValue(key, val));
  }
  *result = local_value;
  return common::Error();
}

IServerInfo* DBConnection::MakeServerInfo(const std::string& content) const {
  return new ServerInfo(content);
}

IDataBaseInfo* DBConnection::MakeDatabaseInfo(const db_name_t& name, bool is_default, size_t size) const {
  return new DataBaseInfo(name, is_default, size);
}

common::Error DBConnection::DelInner(const raw_key_t& key, time_t expiration) {
  return CheckResultCommand(DB_DELETE_KEY_COMMAND,
                            memcached_delete(connection_.handle_, key.data(), key.size(), expiration));
}

common::Error DBConnection::SetInner(const raw_key_t& key,
                                     const raw_value_t& value,
                                     time_t expiration,
                                     uint32_t flags) {
  return CheckResultCommand(DB_SET_KEY_COMMAND, memcached_set(connection_.handle_, key.data(), key.size(), value.data(),
                                                              value.size(), expiration, flags));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  if (!ret_val) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  uint32_t flags = 0;
  memcached_return error;
  size_t value_length = 0;

  char* value = memcached_get(connection_.handle_, key.data(), key.size(), &value_length, &flags, &error);
  common::Error err = CheckResultCommand(DB_GET_KEY_COMMAND, error);
  if (err) {
    return err;
  }

  if (!value) {
    return common::make_error("memcached_get failed.");
  }

  *ret_val = GEN_CMD_STRING_SIZE(value, value_length);
  free(value);
  return common::Error();
}

common::Error DBConnection::ExpireInner(const raw_key_t& key, ttl_t expiration) {
  uint32_t flags = 0;
  memcached_return error;
  size_t value_length = 0;

  char* value = memcached_get(connection_.handle_, key.data(), key.size(), &value_length, &flags, &error);
  common::Error err = CheckResultCommand(DB_SET_TTL_COMMAND, error);
  if (err) {
    return err;
  }

  return CheckResultCommand(DB_SET_TTL_COMMAND, memcached_set(connection_.handle_, key.data(), key.size(), value,
                                                              value_length, expiration, flags));
}

common::Error DBConnection::TTLInner(const raw_key_t& key, ttl_t* expiration) {
  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  time_t exp;
  TTLHolder hld(key, &exp);
  memcached_dump_fn func[1] = {memcached_dump_ttl_callback};
  err = CheckResultCommand(DB_GET_TTL_COMMAND, memcached_dump(connection_.handle_, func, &hld, SIZEOFMASS(func)));
  if (err) {
    return err;
  }

  time_t cur_t = time(nullptr);
  time_t server_t = current_info_.time;
  if (cur_t > exp) {
    if (server_t > exp) {
      *expiration = NO_TTL;
    } else {
      *expiration = EXPIRED_TTL;
    }
  } else {
    *expiration = exp - cur_t;
  }
  return common::Error();
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  ScanHolder hld(cursor_in, pattern, count_keys);
  memcached_dump_fn func[1] = {memcached_dump_scan_callback};
  common::Error err =
      CheckResultCommand(DB_SCAN_COMMAND, memcached_dump(connection_.handle_, func, &hld, SIZEOFMASS(func)));
  if (err) {
    return err;
  }

  *keys_out = hld.result;
  *cursor_out = hld.cursor_out;
  return common::Error();
}

common::Error DBConnection::KeysImpl(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* ret) {
  KeysHolder hld(key_start, key_end, limit);
  memcached_dump_fn func[1] = {memcached_dump_keys_callback};
  common::Error err =
      CheckResultCommand(DB_KEYS_COMMAND, memcached_dump(connection_.handle_, func, &hld, SIZEOFMASS(func)));
  if (err) {
    return err;
  }

  *ret = hld.result;
  return common::Error();
}

common::Error DBConnection::DBKeysCountImpl(keys_limit_t* size) {
  KeysHolder hld(kRangeKeyStart, kRangeKeyEnd, NO_KEYS_LIMIT);
  memcached_dump_fn func[1] = {memcached_dump_keys_callback};
  common::Error err =
      CheckResultCommand(DB_DBKCOUNT_COMMAND, memcached_dump(connection_.handle_, func, &hld, SIZEOFMASS(func)));
  if (err) {
    return err;
  }

  *size = hld.result.size();
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  return CheckResultCommand(DB_FLUSHDB_COMMAND, memcached_flush(connection_.handle_, 0));
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  if (name != GetCurrentDBName()) {
    return ICommandTranslator::InvalidInputArguments(GEN_CMD_STRING(DB_SELECTDB_COMMAND));
  }

  keys_limit_t kcount = 0;
  common::Error err = DBKeysCount(&kcount);
  DCHECK(!err) << err->GetDescription();
  *info = new DataBaseInfo(name, true, kcount);
  return common::Error();
}

common::Error DBConnection::DeleteImpl(const NKeys& keys, NKeys* deleted_keys) {
  for (size_t i = 0; i < keys.size(); ++i) {
    const NKey key = keys[i];
    const auto key_str = key.GetKey();

    common::Error err = DelInner(key_str.GetData(), 0);
    if (err) {
      continue;
    }

    deleted_keys->push_back(key);
  }

  return common::Error();
}

common::Error DBConnection::GetImpl(const NKey& key, NDbKValue* loaded_key) {
  const auto key_str = key.GetKey();

  raw_value_t value_str;
  common::Error err = GetInner(key_str.GetData(), &value_str);
  if (err) {
    return err;
  }

  NValue val(common::Value::CreateStringValue(value_str));
  *loaded_key = NDbKValue(key, val);
  return common::Error();
}

common::Error DBConnection::SetImpl(const NDbKValue& key) {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();

  common::Error err = SetInner(key_str.GetData(), value.GetData(), 0, 0);
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::RenameImpl(const NKey& key, const nkey_t& new_key) {
  const auto key_str = key.GetKey();
  const raw_key_t rkey = key_str.GetData();

  raw_value_t value_str;
  common::Error err = GetInner(rkey, &value_str);
  if (err) {
    return err;
  }

  err = DelInner(rkey, 0);
  if (err) {
    return err;
  }

  const raw_key_t nrkey = new_key.GetData();
  return SetInner(nrkey, value_str, 0, 0);
}

common::Error DBConnection::SetTTLImpl(const NKey& key, ttl_t ttl) {
  const auto key_str = key.GetKey();
  return ExpireInner(key_str.GetData(), ttl);
}

common::Error DBConnection::GetTTLImpl(const NKey& key, ttl_t* ttl) {
  const auto key_str = key.GetKey();
  return TTLInner(key_str.GetData(), ttl);
}

common::Error DBConnection::QuitImpl() {
  common::Error err = Disconnect();
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  std::vector<db_name_t> ldbs = {GetCurrentDBName()};
  *dbs = ldbs;
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, int err) {
  memcached_return_t mem_err = static_cast<memcached_return_t>(err);
  if (mem_err != MEMCACHED_SUCCESS) {
    return GenerateError(cmd, memcached_strerror(connection_.handle_, mem_err));
  }

  return common::Error();
}

}  // namespace memcached
}  // namespace core
}  // namespace fastonosql
