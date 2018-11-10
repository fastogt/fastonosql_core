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

#include <fastonosql/core/db/forestdb/db_connection.h>

#include <libforestdb/forestdb.h>

#include <common/file_system/file_system.h>
#include <common/utils.h>  // for c_strornull

#include <fastonosql/core/db/forestdb/command_translator.h>
#include <fastonosql/core/db/forestdb/database_info.h>
#include "core/db/forestdb/internal/commands_api.h"

namespace fastonosql {
namespace core {
namespace forestdb {
struct fdb {
  fdb_file_handle* handle;
  fdb_kvs_handle* kvs;
  char* db_name;
};

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
                                         CommandHolder(GEN_CMD_STRING(DB_CREATEDB_COMMAND),
                                                       "<name>",
                                                       "Create database",
                                                       UNDEFINED_SINCE,
                                                       DB_CREATEDB_COMMAND " test",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::CreateDatabase),
                                         CommandHolder(GEN_CMD_STRING(DB_REMOVEDB_COMMAND),
                                                       "<name>",
                                                       "Remove database",
                                                       UNDEFINED_SINCE,
                                                       DB_REMOVEDB_COMMAND " test",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::RemoveDatabase),
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
                                                       &CommandsApi::DBkcount),
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
                                         CommandHolder(GEN_CMD_STRING(DB_QUIT_COMMAND),
                                                       "-",
                                                       "Close the connection",
                                                       UNDEFINED_SINCE,
                                                       DB_QUIT_COMMAND,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Quit)};

fdb_status forestdb_create_db(fdb* context, const char* db_name) {
  if (!context || !db_name) {
    return FDB_RESULT_INVALID_ARGS;
  }

  fdb_kvs_config kvs_config = fdb_get_default_kvs_config();
  kvs_config.create_if_missing = true;
  fdb_kvs_handle* kvs = nullptr;
  fdb_status rc = fdb_kvs_open(context->handle, &kvs, db_name, &kvs_config);
  if (rc != FDB_RESULT_SUCCESS) {
    return rc;
  }

  fdb_commit_opt_t opt = FDB_COMMIT_NORMAL;
  fdb_commit(context->handle, opt);
  fdb_kvs_close(kvs);
  return FDB_RESULT_SUCCESS;
}

fdb_status forestdb_remove_db(fdb* context, const char* db_name) {
  if (!context || !db_name) {
    return FDB_RESULT_INVALID_ARGS;
  }

  fdb_status rc = fdb_kvs_remove(context->handle, db_name);
  if (rc != FDB_RESULT_SUCCESS) {
    return rc;
  }

  fdb_commit_opt_t opt = FDB_COMMIT_NORMAL;
  fdb_commit(context->handle, opt);
  return FDB_RESULT_SUCCESS;
}

fdb_status forestdb_select(fdb* context, const char* db_name) {
  if (!context || !db_name) {  // only for named dbs
    return FDB_RESULT_INVALID_ARGS;
  }

  if (context->db_name && strcmp(db_name, context->db_name) == 0) {  // lazy select
    return FDB_RESULT_SUCCESS;
  }

  // open db
  fdb_kvs_config kvs_config = fdb_get_default_kvs_config();
  kvs_config.create_if_missing = false;
  fdb_kvs_handle* kvs = nullptr;
  fdb_status rc = fdb_kvs_open(context->handle, &kvs, db_name, &kvs_config);
  if (rc != FDB_RESULT_SUCCESS) {
    return rc;
  }

  // cleanup old ref
  fdb_commit_opt_t opt = FDB_COMMIT_NORMAL;
  fdb_commit(context->handle, opt);
  common::utils::freeifnotnull(context->db_name);
  context->db_name = nullptr;
  fdb_kvs_close(context->kvs);
  context->kvs = nullptr;

  // assigne new
  context->kvs = kvs;
  context->db_name = common::utils::strdupornull(db_name);
  return FDB_RESULT_SUCCESS;
}

fdb_status forestdb_open(fdb** context, const char* db_path, fdb_config* fconfig) {
  fdb* lcontext = reinterpret_cast<fdb*>(calloc(1, sizeof(fdb)));
  fdb_status rc = fdb_open(&lcontext->handle, db_path, fconfig);
  if (rc != FDB_RESULT_SUCCESS) {
    free(lcontext);
    return rc;
  }

  fdb_kvs_config kvs_config = fdb_get_default_kvs_config();
  rc = fdb_kvs_open_default(lcontext->handle, &lcontext->kvs, &kvs_config);
  if (rc != FDB_RESULT_SUCCESS) {
    free(lcontext);
    return rc;
  }

  *context = lcontext;
  return rc;
}

void forestdb_close(fdb** context) {
  if (!context) {
    return;
  }

  fdb* lcontext = *context;
  if (!lcontext) {
    return;
  }

  fdb_commit_opt_t opt = FDB_COMMIT_NORMAL;
  fdb_commit(lcontext->handle, opt);
  common::utils::freeifnotnull(lcontext->db_name);
  fdb_kvs_close(lcontext->kvs);
  fdb_close(lcontext->handle);
  free(lcontext);
  *context = nullptr;
}

}  // namespace
}  // namespace forestdb

template <>
const char* ConnectionTraits<FORESTDB>::GetBasedOn() {
  return "forestdb";
}

template <>
const char* ConnectionTraits<FORESTDB>::GetVersionApi() {
  return fdb_get_lib_version();
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<FORESTDB>::GetCommands() {
  return forestdb::kCommands;
}

namespace internal {
template <>
common::Error ConnectionAllocatorTraits<forestdb::NativeConnection, forestdb::Config>::Connect(
    const forestdb::Config& config,
    forestdb::NativeConnection** hout) {
  forestdb::NativeConnection* context = nullptr;
  common::Error err = forestdb::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error ConnectionAllocatorTraits<forestdb::NativeConnection, forestdb::Config>::Disconnect(
    forestdb::NativeConnection** handle) {
  forestdb::forestdb_close(handle);
  *handle = nullptr;
  return common::Error();
}

template <>
bool ConnectionAllocatorTraits<forestdb::NativeConnection, forestdb::Config>::IsConnected(
    forestdb::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}

}  // namespace internal

namespace forestdb {

common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  NativeConnection* lcontext = nullptr;
  fdb_config fconfig = fdb_get_default_config();
  std::string db_path = config.db_path;
  std::string folder = common::file_system::get_dir_path(db_path);
  bool is_dir_exist = common::file_system::is_directory_exist(folder);
  if (!is_dir_exist) {
    return common::make_error(common::MemSPrintf("Invalid input path: (%s), please create folder.", folder));
  }

  // fconfig.flags = FDB_OPEN_FLAG_CREATE;
  const char* db_path_ptr = db_path.c_str();  // start point must be file
  fdb_status st = forestdb_open(&lcontext, db_path_ptr, &fconfig);
  if (st != FDB_RESULT_SUCCESS) {
    std::string buff = common::MemSPrintf("Fail open database: %s", fdb_error_msg(st));
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

  forestdb_close(&ldb);
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())) {}

db_name_t DBConnection::GetCurrentDBName() const {
  if (IsConnected()) {  // if connected
    if (connection_.handle_->db_name) {
      return GEN_READABLE_STRING_SIZE(connection_.handle_->db_name, strlen(connection_.handle_->db_name));
    }

    auto conf = GetConfig();
    return common::ConvertToCharBytes(conf->db_name);  // convert from std::string to char bytes
  }

  DNOTREACHED() << "GetCurrentDBName failed!";
  return base_class::GetCurrentDBName();
}

common::Error DBConnection::Info(const command_buffer_t& args, ServerInfo::Stats* statsout) {
  UNUSED(args);
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

common::Error DBConnection::SetInner(const raw_key_t& key, const raw_value_t& value) {
  return CheckResultCommand(DB_SET_KEY_COMMAND,
                            fdb_set_kv(connection_.handle_->kvs, key.data(), key.size(), value.data(), value.size()));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  void* value_out = nullptr;
  size_t valuelen_out = 0;
  common::Error err = CheckResultCommand(
      DB_GET_KEY_COMMAND, fdb_get_kv(connection_.handle_->kvs, key.data(), key.size(), &value_out, &valuelen_out));
  if (err) {
    return err;
  }

  *ret_val = GEN_CMD_STRING_SIZE(static_cast<const raw_value_t::value_type*>(value_out), valuelen_out);
  return common::Error();
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  command_buffer_t exist_key;
  common::Error err = GetInner(key, &exist_key);
  if (err) {
    return err;
  }

  return CheckResultCommand(DB_DELETE_KEY_COMMAND, fdb_del_kv(connection_.handle_->kvs, key.data(), key.size()));
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  fdb_iterator* it = nullptr;
  fdb_iterator_opt_t opt = FDB_ITR_NONE;

  common::Error err = CheckResultCommand(DB_SCAN_COMMAND,
                                         fdb_iterator_init(connection_.handle_->kvs, &it, nullptr, 0, nullptr, 0, opt));
  if (err) {
    return err;
  }

  fdb_doc* doc = nullptr;
  cursor_t offset_pos = cursor_in;
  cursor_t lcursor_out = 0;
  std::vector<command_buffer_t> lkeys_out;
  do {
    fdb_status rc = fdb_iterator_get(it, &doc);
    if (rc != FDB_RESULT_SUCCESS) {
      break;
    }

    if (lkeys_out.size() < count_keys) {
      if (IsKeyMatchPattern(static_cast<const char*>(doc->key), doc->keylen, pattern)) {
        if (offset_pos == 0) {
          lkeys_out.push_back(
              GEN_READABLE_STRING_SIZE(static_cast<const raw_key_t::value_type*>(doc->key), doc->keylen));
        } else {
          offset_pos--;
        }
      }
    } else {
      lcursor_out = cursor_in + count_keys;
      break;
    }
    fdb_doc_free(doc);
  } while (fdb_iterator_next(it) != FDB_RESULT_ITERATOR_FAIL);
  fdb_iterator_close(it);

  *keys_out = lkeys_out;
  *cursor_out = lcursor_out;
  return common::Error();
}

common::Error DBConnection::KeysImpl(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* ret) {
  fdb_iterator* it = nullptr;
  fdb_iterator_opt_t opt = FDB_ITR_NONE;
  const char* key_start_ptr = key_start == kRangeKeyStart ? nullptr : key_start.data();
  const size_t key_start_size = key_start == kRangeKeyStart ? 0 : key_start.size();
  const char* key_end_ptr = key_end == kRangeKeyEnd ? nullptr : key_end.data();
  const size_t key_end_size = key_end == kRangeKeyEnd ? 0 : key_end.size();

  common::Error err = CheckResultCommand(
      DB_KEYS_COMMAND,
      fdb_iterator_init(connection_.handle_->kvs, &it, key_start_ptr, key_start_size, key_end_ptr, key_end_size, opt));
  if (err) {
    return err;
  }

  fdb_doc* doc = nullptr;
  do {
    fdb_status rc = fdb_iterator_get(it, &doc);
    if (rc != FDB_RESULT_SUCCESS) {
      break;
    }

    if (ret->size() < limit) {
      const raw_key_t key = GEN_CMD_STRING_SIZE(static_cast<const char*>(doc->key), doc->keylen);
      if (IsKeyInEndRange(key, key_end)) {
        ret->push_back(key);
      }
    } else {
      break;
    }
    fdb_doc_free(doc);
  } while (fdb_iterator_next(it) != FDB_RESULT_ITERATOR_FAIL);
  fdb_iterator_close(it);
  return common::Error();
}

common::Error DBConnection::DBkcountImpl(keys_limit_t* size) {
  fdb_iterator* it = nullptr;
  fdb_iterator_opt_t opt = FDB_ITR_NONE;

  common::Error err = CheckResultCommand(DB_DBKCOUNT_COMMAND,
                                         fdb_iterator_init(connection_.handle_->kvs, &it, nullptr, 0, nullptr, 0, opt));
  if (err) {
    return err;
  }

  keys_limit_t sz = 0;
  fdb_doc* doc = nullptr;
  do {
    fdb_status rc = fdb_iterator_get(it, &doc);
    if (rc != FDB_RESULT_SUCCESS) {
      break;
    }

    sz++;
    fdb_doc_free(doc);
  } while (fdb_iterator_next(it) != FDB_RESULT_ITERATOR_FAIL);
  fdb_iterator_close(it);

  *size = sz;
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  fdb_iterator* it = nullptr;
  fdb_iterator_opt_t opt = FDB_ITR_NONE;

  common::Error err = CheckResultCommand(DB_FLUSHDB_COMMAND,
                                         fdb_iterator_init(connection_.handle_->kvs, &it, nullptr, 0, nullptr, 0, opt));
  if (err) {
    return err;
  }

  fdb_doc* doc = nullptr;
  do {
    fdb_status rc = fdb_iterator_get(it, &doc);
    if (rc != FDB_RESULT_SUCCESS) {
      break;
    }

    std::string key;
    err = CheckResultCommand(DB_FLUSHDB_COMMAND, fdb_del_kv(connection_.handle_->kvs, key.c_str(), key.size()));
    if (err) {
      fdb_iterator_close(it);
      return err;
    }
    fdb_doc_free(doc);
  } while (fdb_iterator_next(it) != FDB_RESULT_ITERATOR_FAIL);
  fdb_iterator_close(it);

  return common::Error();
}

common::Error DBConnection::CreateDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  auto conf = GetConfig();
  const std::string db_name = common::ConvertToString(name);
  common::Error err =
      CheckResultCommand(DB_CREATEDB_COMMAND, forestdb_create_db(connection_.handle_, db_name.data()));  // safe
  if (err) {
    return err;
  }

  *info = new DataBaseInfo(name, false, 0);
  return common::Error();
}

common::Error DBConnection::RemoveDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  auto conf = GetConfig();
  const std::string db_name = common::ConvertToString(name);
  common::Error err =
      CheckResultCommand(DB_REMOVEDB_COMMAND, forestdb_remove_db(connection_.handle_, db_name.data()));  // safe
  if (err) {
    return err;
  }

  *info = new DataBaseInfo(name, false, 0);
  return common::Error();
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  auto conf = GetConfig();
  const std::string db_name = common::ConvertToString(name);
  common::Error err =
      CheckResultCommand(DB_SELECTDB_COMMAND, forestdb_select(connection_.handle_, db_name.data()));  // safe
  if (err) {
    return err;
  }

  connection_.config_->db_name = db_name;
  keys_limit_t kcount = 0;
  err = DBkcount(&kcount);
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

common::Error DBConnection::RenameImpl(const NKey& key, const nkey_t& new_key) {
  const auto key_str = key.GetKey();
  const raw_key_t rkey = key_str.GetData();

  raw_value_t value_str;
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

common::Error DBConnection::QuitImpl() {
  common::Error err = Disconnect();
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  fdb_kvs_name_list forestdb_dbs;
  common::Error err =
      CheckResultCommand("CONFIG GET DATABASES", fdb_get_kvs_name_list(connection_.handle_->handle, &forestdb_dbs));
  if (err) {
    return err;
  }

  db_names_t ldbs;
  for (size_t i = 0; i < forestdb_dbs.num_kvs_names; ++i) {
    ldbs.push_back(common::ConvertToCharBytes(forestdb_dbs.kvs_names[i]));
  }

  err = CheckResultCommand("CONFIG GET DATABASES", fdb_free_kvs_name_list(&forestdb_dbs));
  if (err) {
    return err;
  }

  *dbs = ldbs;
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, fdb_status err) {
  if (err != FDB_RESULT_SUCCESS) {
    return GenerateError(cmd, fdb_error_msg(err));
  }

  return common::Error();
}

}  // namespace forestdb
}  // namespace core
}  // namespace fastonosql
