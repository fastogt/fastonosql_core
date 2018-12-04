/*  Copyright (C) 2014-2018 FastoGT. All right reserved.

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

#include <fastonosql/core/db/unqlite/db_connection.h>

extern "C" {
#include <unqlite.h>
}

#include <common/file_system/file_system.h>
#include <common/file_system/string_path_utils.h>

#include <fastonosql/core/db/unqlite/command_translator.h>
#include <fastonosql/core/db/unqlite/database_info.h>
#include "core/db/unqlite/internal/commands_api.h"

namespace {

std::string unqlite_strerror(int unqlite_error) {
  if (unqlite_error == UNQLITE_OK) {
    return std::string();
  } else if (unqlite_error == UNQLITE_NOMEM) {
    return "Out of memory";
  } else if (unqlite_error == UNQLITE_ABORT) {
    return "Another thread have released this instance";
  } else if (unqlite_error == UNQLITE_IOERR) {
    return "IO error";
  } else if (unqlite_error == UNQLITE_CORRUPT) {
    return "Corrupt pointer";
  } else if (unqlite_error == UNQLITE_LOCKED) {
    return "Forbidden Operation";
  } else if (unqlite_error == UNQLITE_BUSY) {
    return "The database file is locked";
  } else if (unqlite_error == UNQLITE_DONE) {
    return "Operation done";
  } else if (unqlite_error == UNQLITE_PERM) {
    return "Permission error";
  } else if (unqlite_error == UNQLITE_NOTIMPLEMENTED) {
    return "Method not implemented by the underlying "
           "Key/Value storage engine";
  } else if (unqlite_error == UNQLITE_NOTFOUND) {
    return "No such record";
  } else if (unqlite_error == UNQLITE_NOOP) {
    return "No such method";
  } else if (unqlite_error == UNQLITE_INVALID) {
    return "Invalid parameter";
  } else if (unqlite_error == UNQLITE_EOF) {
    return "End Of Input";
  } else if (unqlite_error == UNQLITE_UNKNOWN) {
    return "Unknown configuration option";
  } else if (unqlite_error == UNQLITE_LIMIT) {
    return "Database limit reached";
  } else if (unqlite_error == UNQLITE_EXISTS) {
    return "Record exists";
  } else if (unqlite_error == UNQLITE_EMPTY) {
    return "Empty record";
  } else if (unqlite_error == UNQLITE_COMPILE_ERR) {
    return "Compilation error";
  } else if (unqlite_error == UNQLITE_VM_ERR) {
    return "Virtual machine error";
  } else if (unqlite_error == UNQLITE_FULL) {
    return "Full database (unlikely)";
  } else if (unqlite_error == UNQLITE_CANTOPEN) {
    return "Unable to open the database file";
  } else if (unqlite_error == UNQLITE_READ_ONLY) {
    return "Read only Key/Value storage engine";
  } else if (unqlite_error == UNQLITE_LOCKERR) {
    return "Locking protocol error";
  } else {
    return common::MemSPrintf("Unknown error %d", unqlite_error);
  }
}

int unqlite_data_callback_get_value(const void* data, unsigned int size, void* str) {
  typedef fastonosql::core::raw_value_t raw_value_t;
  raw_value_t* out = static_cast<raw_value_t*>(str);
  const raw_value_t::value_type* begin = static_cast<const raw_value_t::value_type*>(data);
  out->assign(begin, begin + size);
  return UNQLITE_OK;
}

int unqlite_data_callback_get_key(const void* data, unsigned int size, void* str) {
  typedef fastonosql::core::raw_key_t raw_key_t;
  raw_key_t* out = static_cast<raw_key_t*>(str);
  const raw_key_t::value_type* begin = static_cast<const raw_key_t::value_type*>(data);
  out->assign(begin, begin + size);
  return UNQLITE_OK;
}

}  // namespace

namespace fastonosql {
namespace core {
namespace unqlite {
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
                                                       1,
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
}  // namespace unqlite

template <>
const char* ConnectionTraits<UNQLITE>::GetBasedOn() {
  return "unqlite";
}

template <>
const char* ConnectionTraits<UNQLITE>::GetVersionApi() {
  return UNQLITE_VERSION;
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<UNQLITE>::GetCommands() {
  return unqlite::kCommands;
}
namespace internal {
template <>
common::Error Connection<unqlite::NativeConnection, unqlite::Config>::Connect(const unqlite::Config& config,
                                                                              unqlite::NativeConnection** hout) {
  unqlite::NativeConnection* context = nullptr;
  common::Error err = unqlite::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}
template <>
common::Error Connection<unqlite::NativeConnection, unqlite::Config>::Disconnect(unqlite::NativeConnection** handle) {
  unqlite_close(*handle);
  *handle = nullptr;
  return common::Error();
}
template <>
bool Connection<unqlite::NativeConnection, unqlite::Config>::IsConnected(unqlite::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}

}  // namespace internal
namespace unqlite {

common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  struct unqlite* lcontext = nullptr;
  std::string db_path = config.db_path;
  std::string folder = common::file_system::get_dir_path(db_path);
  bool is_dir_exist = common::file_system::is_directory_exist(folder);
  if (!is_dir_exist) {
    return common::make_error(common::MemSPrintf("Invalid input path: (%s), please create folder.", folder));
  }

  const char* db_path_ptr = db_path.c_str();
  unsigned int env_flags = config.env_flags;
  int st = unqlite_open(&lcontext, db_path_ptr, env_flags);
  if (st != UNQLITE_OK) {
    std::string buff = common::MemSPrintf("Failed to open database error: %s!", unqlite_strerror(st));
    return common::make_error(buff);
  }

  *context = lcontext;
  return common::Error();
}

common::Error TestConnection(const Config& config) {
  NativeConnection* ldb = nullptr;
  common::Error err = CreateConnection(config, &ldb);
  if (err) {
    return err;
  }

  unqlite_close(ldb);
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())) {}

common::Error DBConnection::Info(ServerInfo::Stats* statsout) {
  if (!statsout) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  ServerInfo::Stats linfo;
  auto conf = GetConfig();
  linfo.db_path = conf->db_path;
  off_t sz;
  common::ErrnoError errn = common::file_system::get_file_size_by_path(linfo.db_path, &sz);
  if (!errn) {
    linfo.db_size = sz;
  }

  *statsout = linfo;
  return common::Error();
}

IServerInfo* DBConnection::MakeServerInfo(const std::string& content) const {
  return new ServerInfo(content);
}

IDataBaseInfo* DBConnection::MakeDatabaseInfo(const db_name_t& name, bool is_default, size_t size) const {
  return new DataBaseInfo(name, is_default, size);
}

common::Error DBConnection::SetInner(const raw_key_t& key, const raw_value_t& value) {
  return CheckResultCommand(DB_SET_KEY_COMMAND,
                            unqlite_kv_store(connection_.handle_, key.data(), key.size(), value.data(), value.size()));
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  return CheckResultCommand(DB_DELETE_KEY_COMMAND, unqlite_kv_delete(connection_.handle_, key.data(), key.size()));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  return CheckResultCommand(DB_GET_KEY_COMMAND, unqlite_kv_fetch_callback(connection_.handle_, key.data(), key.size(),
                                                                          unqlite_data_callback_get_value, ret_val));
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  unqlite_kv_cursor* cursor; /* Cursor handle */
  common::Error err = CheckResultCommand(DB_SCAN_COMMAND, unqlite_kv_cursor_init(connection_.handle_, &cursor));
  if (err) {
    return err;
  }
  /* Point to the first record */
  unqlite_kv_cursor_first_entry(cursor);

  /* Iterate over the entries */
  cursor_t offset_pos = cursor_in;
  cursor_t lcursor_out = 0;
  std::vector<command_buffer_t> lkeys_out;
  while (unqlite_kv_cursor_valid_entry(cursor)) {
    if (lkeys_out.size() < count_keys) {
      raw_key_t skey;
      unqlite_kv_cursor_key_callback(cursor, unqlite_data_callback_get_key, &skey);
      if (IsKeyMatchPattern(skey.data(), skey.size(), pattern)) {
        if (offset_pos == 0) {
          lkeys_out.push_back(skey);
        } else {
          offset_pos--;
        }
      }
    } else {
      lcursor_out = cursor_in + count_keys;
      break;
    }

    /* Point to the next entry */
    unqlite_kv_cursor_next_entry(cursor);
  }
  /* Finally, Release our cursor */
  unqlite_kv_cursor_release(connection_.handle_, cursor);

  *keys_out = lkeys_out;
  *cursor_out = lcursor_out;
  return common::Error();
}

common::Error DBConnection::KeysImpl(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* out) { /* Allocate a new cursor instance */
  unqlite_kv_cursor* cursor;                            /* Cursor handle */
  common::Error err = CheckResultCommand(DB_KEYS_COMMAND, unqlite_kv_cursor_init(connection_.handle_, &cursor));
  if (err) {
    return err;
  }
  /* Point to the first record */
  unqlite_kv_cursor_first_entry(cursor);

  /* Iterate over the entries */
  std::vector<command_buffer_t> result;
  while (unqlite_kv_cursor_valid_entry(cursor) && limit > result.size()) {
    raw_key_t key;
    unqlite_kv_cursor_key_callback(cursor, unqlite_data_callback_get_key, &key);
    if (IsKeyInRange(key_start, key, key_end)) {
      result.push_back(key);
    }

    /* Point to the next entry */
    unqlite_kv_cursor_next_entry(cursor);
  }
  /* Finally, Release our cursor */
  unqlite_kv_cursor_release(connection_.handle_, cursor);

  *out = result;
  return common::Error();
}

common::Error DBConnection::DBKeysCountImpl(keys_limit_t* size) {
  /* Allocate a new cursor instance */
  unqlite_kv_cursor* cursor; /* Cursor handle */
  common::Error err = CheckResultCommand(DB_DBKCOUNT_COMMAND, unqlite_kv_cursor_init(connection_.handle_, &cursor));
  if (err) {
    return err;
  }
  /* Point to the first record */
  unqlite_kv_cursor_first_entry(cursor);

  keys_limit_t sz = 0;
  /* Iterate over the entries */
  while (unqlite_kv_cursor_valid_entry(cursor)) {
    sz++;
    /* Point to the next entry */
    unqlite_kv_cursor_next_entry(cursor);
  }
  /* Finally, Release our cursor */
  unqlite_kv_cursor_release(connection_.handle_, cursor);

  *size = sz;
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  unqlite_kv_cursor* cursor; /* Cursor handle */
  common::Error err = CheckResultCommand(DB_FLUSHDB_COMMAND, unqlite_kv_cursor_init(connection_.handle_, &cursor));
  if (err) {
    return err;
  }
  /* Point to the first record */
  unqlite_kv_cursor_first_entry(cursor);

  /* Iterate over the entries */
  while (unqlite_kv_cursor_valid_entry(cursor)) {
    raw_key_t key;
    unqlite_kv_cursor_key_callback(cursor, unqlite_data_callback_get_key, &key);
    common::Error err = DelInner(key);
    if (err) {
      return err;
    }
    /* Point to the next entry */
    unqlite_kv_cursor_next_entry(cursor);
  }

  /* Finally, Release our cursor */
  unqlite_kv_cursor_release(connection_.handle_, cursor);
  return common::Error();
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

common::Error DBConnection::SetImpl(const NDbKValue& key) {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();

  common::Error err = SetInner(key_str.GetData(), value.GetData());
  if (err) {
    return err;
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

common::Error DBConnection::RenameImpl(const NKey& key, const nkey_t& new_key) {
  const auto key_str = key.GetKey();
  const raw_key_t rkey = key_str.GetData();
  command_buffer_t value_str;

  common::Error err = GetInner(rkey, &value_str);
  if (err) {
    return err;
  }

  err = DelInner(rkey);
  if (err) {
    return err;
  }

  const raw_key_t nkey = new_key.GetData();
  return SetInner(nkey, value_str);
}

common::Error DBConnection::DeleteImpl(const NKeys& keys, NKeys* deleted_keys) {
  for (size_t i = 0; i < keys.size(); ++i) {
    const NKey key = keys[i];
    const auto key_str = key.GetKey();

    common::Error err = DelInner(key_str.GetData());
    if (err) {
      continue;
    }

    deleted_keys->push_back(key);
  }

  return common::Error();
}

common::Error DBConnection::QuitImpl() {
  return Disconnect();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  db_names_t ldbs = {GetCurrentDBName()};
  *dbs = ldbs;
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, int err) {
  if (err != UNQLITE_OK) {
    return GenerateError(cmd, unqlite_strerror(err));
  }

  return common::Error();
}

}  // namespace unqlite
}  // namespace core
}  // namespace fastonosql
