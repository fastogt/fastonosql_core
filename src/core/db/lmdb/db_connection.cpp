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

#include <fastonosql/core/db/lmdb/db_connection.h>

#include <lmdb.h>  // for mdb_txn_abort, MDB_val

#include <common/utils.h>  // for c_strornull

#include <fastonosql/core/db/lmdb/command_translator.h>
#include <fastonosql/core/db/lmdb/config.h>  // for Config
#include <fastonosql/core/db/lmdb/database_info.h>
#include "core/db/lmdb/internal/commands_api.h"

#define LMDB_OK 0

namespace fastonosql {
namespace core {
namespace lmdb {
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
                                         CommandHolder(GEN_CMD_STRING(LMDB_DROPDB_COMMAND),
                                                       "-",
                                                       "Drop database",
                                                       UNDEFINED_SINCE,
                                                       LMDB_DROPDB_COMMAND,
                                                       0,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::DropDatabase),
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
}
}  // namespace lmdb
template <>
const char* ConnectionTraits<LMDB>::GetBasedOn() {
  return "lmdb";
}

template <>
const char* ConnectionTraits<LMDB>::GetVersionApi() {
  return STRINGIZE(MDB_VERSION_MAJOR) "." STRINGIZE(MDB_VERSION_MINOR) "." STRINGIZE(MDB_VERSION_PATCH);
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<LMDB>::GetCommands() {
  return lmdb::kCommands;
}

namespace {
MDB_val ConvertToLMDBSlice(const readable_string_t::value_type* key, size_t len) {
  MDB_val mkey;
  mkey.mv_size = len;
  mkey.mv_data = const_cast<readable_string_t::value_type*>(key);
  return mkey;
}
}  // namespace
namespace lmdb {
struct lmdb {
  MDB_env* env;
  MDB_dbi dbi;
  char* db_name;
};

namespace {

unsigned int lmdb_db_flag_from_env_flags(int env_flags) {
  return (env_flags & MDB_RDONLY) ? MDB_RDONLY : 0;
}

int lmdb_create_db(lmdb* context, const char* db_name, int env_flags) {
  if (!context || !db_name) {
    return EINVAL;
  }

  MDB_txn* txn = nullptr;
  int rc = mdb_txn_begin(context->env, nullptr, lmdb_db_flag_from_env_flags(env_flags), &txn);
  if (rc != LMDB_OK) {
    return rc;
  }

  MDB_dbi ldbi = 0;
  unsigned int flg = (env_flags & MDB_RDONLY) ? 0 : MDB_CREATE;
  rc = mdb_dbi_open(txn, db_name, flg, &ldbi);
  if (rc != LMDB_OK) {
    mdb_txn_abort(txn);
    return rc;
  }

  mdb_txn_commit(txn);
  mdb_dbi_close(context->env, ldbi);
  return LMDB_OK;
}

int lmdb_remove_db(lmdb* context, const char* db_name, int env_flags) {
  if (!context || !db_name) {
    return EINVAL;
  }

  const unsigned int flg = lmdb_db_flag_from_env_flags(env_flags);
  MDB_dbi ldbi = 0;
  MDB_txn* txn = nullptr;
  int rc = mdb_txn_begin(context->env, nullptr, flg, &txn);
  if (rc != LMDB_OK) {
    return rc;
  }

  rc = mdb_dbi_open(txn, db_name, flg, &ldbi);
  if (rc != LMDB_OK) {
    mdb_txn_abort(txn);
    return rc;
  }

  rc = mdb_drop(txn, ldbi, 1);
  if (rc != LMDB_OK) {
    mdb_txn_abort(txn);
    return rc;
  }

  return mdb_txn_commit(txn);
}

int lmdb_select(lmdb* context, const char* db_name, int env_flags) {
  if (!context || !db_name) {  // only for named dbs
    return EINVAL;
  }

  if (context->db_name && strcmp(db_name, context->db_name) == 0) {  // lazy select
    return LMDB_OK;
  }

  // open db
  MDB_txn* txn = nullptr;
  int rc = mdb_txn_begin(context->env, nullptr, lmdb_db_flag_from_env_flags(env_flags), &txn);
  if (rc != LMDB_OK) {
    return rc;
  }

  MDB_dbi ldbi = 0;
  const unsigned int flg = (env_flags & MDB_RDONLY) ? 0 : MDB_CREATE;
  rc = mdb_dbi_open(txn, db_name, flg, &ldbi);
  if (rc != LMDB_OK) {
    mdb_txn_abort(txn);
    return rc;
  }

  rc = mdb_txn_commit(txn);
  if (rc != LMDB_OK) {
    return rc;
  }

  // cleanup old ref
  common::utils::freeifnotnull(context->db_name);
  context->db_name = nullptr;
  mdb_dbi_close(context->env, context->dbi);
  context->dbi = 0;

  // assigne new
  context->dbi = ldbi;
  context->db_name = common::utils::strdupornull(db_name);
  return LMDB_OK;
}

int lmdb_open(lmdb** context, const char* db_path, unsigned int env_flags, MDB_dbi max_dbs) {
  lmdb* lcontext = reinterpret_cast<lmdb*>(calloc(1, sizeof(lmdb)));
  int rc = mdb_env_create(&lcontext->env);
  if (rc != LMDB_OK) {
    free(lcontext);
    return rc;
  }

  rc = mdb_env_set_maxdbs(lcontext->env, max_dbs);
  if (rc != LMDB_OK) {
    free(lcontext);
    return rc;
  }

  rc = mdb_env_open(lcontext->env, db_path, env_flags, 0664);
  if (rc != LMDB_OK) {
    free(lcontext);
    return rc;
  }

  *context = lcontext;
  return rc;
}

void lmdb_close(lmdb** context) {
  if (!context) {
    return;
  }

  lmdb* lcontext = *context;
  if (!lcontext) {
    return;
  }

  common::utils::freeifnotnull(lcontext->db_name);
  lcontext->db_name = nullptr;
  mdb_dbi_close(lcontext->env, lcontext->dbi);
  lcontext->dbi = 0;
  mdb_env_close(lcontext->env);
  lcontext->env = nullptr;
  free(lcontext);
  *context = nullptr;
}

}  // namespace
}  // namespace lmdb
namespace internal {
template <>
common::Error ConnectionAllocatorTraits<lmdb::NativeConnection, lmdb::Config>::Connect(const lmdb::Config& config,
                                                                                       lmdb::NativeConnection** hout) {
  lmdb::NativeConnection* context = nullptr;
  common::Error err = lmdb::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error ConnectionAllocatorTraits<lmdb::NativeConnection, lmdb::Config>::Disconnect(
    lmdb::NativeConnection** handle) {
  lmdb::lmdb_close(handle);
  *handle = nullptr;
  return common::Error();
}

template <>
bool ConnectionAllocatorTraits<lmdb::NativeConnection, lmdb::Config>::IsConnected(lmdb::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}

}  // namespace internal

namespace lmdb {

common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context || config.db_name.empty() || config.db_path.empty()) {  // only named dbs
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  lmdb* lcontext = nullptr;
  std::string db_path = config.db_path;
  std::string folder = common::file_system::get_dir_path(db_path);
  bool is_dir_exist = common::file_system::is_directory_exist(folder);
  if (!is_dir_exist) {
    return common::make_error(
        common::MemSPrintf("Invalid input path: (%s), There is already a "
                           "directory as the file name you specified, "
                           "specify a different name.",
                           folder));
  }

  bool is_single_file = config.IsSingleFileDB();
  common::tribool is_dir = common::file_system::is_directory(db_path);
  if (is_dir == common::SUCCESS) {
    if (is_single_file) {  // if dir but want single file
      return common::make_error(common::MemSPrintf("Invalid input path: (%s), please remove folder.", db_path));
    }
  } else if (is_dir == common::FAIL) {  // if file
    if (!is_single_file) {              // if file but want dir
      return common::make_error(
          common::MemSPrintf("Invalid input path: (%s), There is already a "
                             "file as the folder name you specified, "
                             "specify a different name.",
                             db_path));
    }
  } else {                  // if nothing created
    if (!is_single_file) {  // if file but want dir
      return common::make_error(common::MemSPrintf("Invalid input path: (%s), please create folder.", db_path));
    }
  }

  const char* db_path_ptr = db_path.c_str();
  unsigned int env_flags = config.env_flags;
  unsigned int max_dbs = config.max_dbs;
  int st = lmdb_open(&lcontext, db_path_ptr, env_flags, max_dbs);
  if (st != LMDB_OK) {
    std::string buff = common::MemSPrintf("Fail open database: %s", mdb_strerror(st));
    return common::make_error(buff);
  }

  const char* db_name = config.db_name.c_str();
  st = lmdb_select(lcontext, db_name, env_flags);
  if (st != LMDB_OK) {
    lmdb_close(&lcontext);
    std::string buff = common::MemSPrintf("Failed select database: %s", mdb_strerror(st));
    return common::make_error(buff);
  }

  *context = lcontext;
  return common::Error();
}  // namespace lmdb

common::Error TestConnection(const Config& config) {
  NativeConnection* ldb = nullptr;
  common::Error err = CreateConnection(config, &ldb);
  if (err) {
    return err;
  }

  lmdb_close(&ldb);
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())) {}

DBConnection::~DBConnection() {}

db_name_t DBConnection::GetCurrentDBName() const {
  if (IsConnected()) {  // if connected
    if (connection_.handle_->db_name) {
      return common::ConvertToCharBytes(connection_.handle_->db_name);
    }

    auto conf = GetConfig();
    return common::ConvertToCharBytes(conf->db_name);
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

  *statsout = linfo;
  return common::Error();
}

common::Error DBConnection::DropDatabase() {
  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  MDB_txn* txn = nullptr;
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  err = CheckResultCommand(LMDB_DROPDB_COMMAND, mdb_txn_begin(connection_.handle_->env, nullptr,
                                                              lmdb_db_flag_from_env_flags(env_flags), &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(LMDB_DROPDB_COMMAND, mdb_drop(txn, connection_.handle_->dbi, 1));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  return CheckResultCommand(LMDB_DROPDB_COMMAND, mdb_txn_commit(txn));
}

common::Error DBConnection::SetInner(const raw_value_t& key, const raw_value_t& value) {
  MDB_val key_slice = ConvertToLMDBSlice(key.data(), key.size());
  MDB_val val_slice = ConvertToLMDBSlice(value.data(), value.size());

  MDB_txn* txn = nullptr;
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  common::Error err = CheckResultCommand(
      DB_SET_KEY_COMMAND,
      mdb_txn_begin(connection_.handle_->env, nullptr, lmdb_db_flag_from_env_flags(env_flags), &txn));
  if (err) {
    return err;
  }
  err = CheckResultCommand(DB_SET_KEY_COMMAND, mdb_put(txn, connection_.handle_->dbi, &key_slice, &val_slice, 0));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  return CheckResultCommand(DB_SET_KEY_COMMAND, mdb_txn_commit(txn));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  MDB_val key_slice = ConvertToLMDBSlice(key.data(), key.size());
  MDB_val mval;

  MDB_txn* txn = nullptr;
  common::Error err =
      CheckResultCommand(DB_GET_KEY_COMMAND, mdb_txn_begin(connection_.handle_->env, nullptr, MDB_RDONLY, &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(DB_GET_KEY_COMMAND, mdb_get(txn, connection_.handle_->dbi, &key_slice, &mval));
  mdb_txn_abort(txn);
  if (err) {
    return err;
  }

  *ret_val = GEN_CMD_STRING_SIZE(reinterpret_cast<const raw_value_t::value_type*>(mval.mv_data), mval.mv_size);
  return common::Error();
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  MDB_val key_slice = ConvertToLMDBSlice(key.data(), key.size());

  MDB_txn* txn = nullptr;
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  common::Error err = CheckResultCommand(
      DB_DELETE_KEY_COMMAND,
      mdb_txn_begin(connection_.handle_->env, nullptr, lmdb_db_flag_from_env_flags(env_flags), &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(DB_DELETE_KEY_COMMAND, mdb_del(txn, connection_.handle_->dbi, &key_slice, nullptr));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  return CheckResultCommand(DB_DELETE_KEY_COMMAND, mdb_txn_commit(txn));
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  MDB_cursor* cursor = nullptr;
  MDB_txn* txn = nullptr;
  common::Error err =
      CheckResultCommand(DB_SCAN_COMMAND, mdb_txn_begin(connection_.handle_->env, nullptr, MDB_RDONLY, &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(DB_SCAN_COMMAND, mdb_cursor_open(txn, connection_.handle_->dbi, &cursor));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  MDB_val key;
  MDB_val data;
  uint64_t offset_pos = cursor_in;
  uint64_t lcursor_out = 0;
  std::vector<command_buffer_t> lkeys_out;
  while ((mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == LMDB_OK)) {
    if (lkeys_out.size() < count_keys) {
      command_buffer_t skey =
          GEN_CMD_STRING_SIZE(reinterpret_cast<const command_buffer_char_t*>(key.mv_data), key.mv_size);
      if (common::MatchPattern(skey, pattern)) {
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
  }

  *keys_out = lkeys_out;
  *cursor_out = lcursor_out;
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);
  return common::Error();
}

common::Error DBConnection::KeysImpl(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* ret) {
  MDB_cursor* cursor = nullptr;
  MDB_txn* txn = nullptr;
  common::Error err =
      CheckResultCommand(DB_KEYS_COMMAND, mdb_txn_begin(connection_.handle_->env, nullptr, MDB_RDONLY, &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(DB_KEYS_COMMAND, mdb_cursor_open(txn, connection_.handle_->dbi, &cursor));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  MDB_val key;
  MDB_val data;
  while ((mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == LMDB_OK) && limit > ret->size()) {
    command_buffer_t skey =
        GEN_CMD_STRING_SIZE(reinterpret_cast<const command_buffer_t::value_type*>(key.mv_data), key.mv_size);
    if (key_start < skey && key_end > skey) {
      ret->push_back(skey);
    }
  }

  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);
  return common::Error();
}

common::Error DBConnection::DBkcountImpl(keys_limit_t* size) {
  MDB_cursor* cursor = nullptr;
  MDB_txn* txn = nullptr;
  common::Error err =
      CheckResultCommand(DB_DBKCOUNT_COMMAND, mdb_txn_begin(connection_.handle_->env, nullptr, MDB_RDONLY, &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(DB_DBKCOUNT_COMMAND, mdb_cursor_open(txn, connection_.handle_->dbi, &cursor));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  MDB_val key;
  MDB_val data;
  size_t sz = 0;
  while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == LMDB_OK) {
    sz++;
  }
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);

  *size = sz;
  return common::Error();
}

common::Error DBConnection::CreateDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  const std::string db_name = common::ConvertToString(name);
  common::Error err =
      CheckResultCommand(DB_CREATEDB_COMMAND, lmdb_create_db(connection_.handle_, db_name.data(), env_flags));  // safe
  if (err) {
    return err;
  }

  *info = new DataBaseInfo(name, false, 0);
  return common::Error();
}

common::Error DBConnection::RemoveDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  const std::string db_name = common::ConvertToString(name);
  common::Error err =
      CheckResultCommand(DB_REMOVEDB_COMMAND, lmdb_remove_db(connection_.handle_, db_name.data(), env_flags));  // safe
  if (err) {
    return err;
  }

  *info = new DataBaseInfo(name, false, 0);
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  MDB_cursor* cursor = nullptr;
  MDB_txn* txn = nullptr;
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  common::Error err = CheckResultCommand(
      DB_FLUSHDB_COMMAND,
      mdb_txn_begin(connection_.handle_->env, nullptr, lmdb_db_flag_from_env_flags(env_flags), &txn));
  if (err) {
    return err;
  }

  err = CheckResultCommand(DB_FLUSHDB_COMMAND, mdb_cursor_open(txn, connection_.handle_->dbi, &cursor));
  if (err) {
    mdb_txn_abort(txn);
    return err;
  }

  MDB_val key;
  MDB_val data;
  size_t sz = 0;
  while (mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == LMDB_OK) {
    sz++;
    err = CheckResultCommand(DB_FLUSHDB_COMMAND, mdb_del(txn, connection_.handle_->dbi, &key, nullptr));
    if (err) {
      mdb_cursor_close(cursor);
      mdb_txn_abort(txn);
      return err;
    }
  }

  mdb_cursor_close(cursor);
  if (sz != 0) {
    return CheckResultCommand(DB_DELETE_KEY_COMMAND, mdb_txn_commit(txn));
  }

  mdb_txn_abort(txn);
  return common::Error();
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  auto conf = GetConfig();
  int env_flags = conf->env_flags;
  const std::string db_name = common::ConvertToString(name);
  common::Error err =
      CheckResultCommand(DB_SELECTDB_COMMAND, lmdb_select(connection_.handle_, db_name.data(), env_flags));  // safe
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
  const raw_key_t rkey = key_str.GetData();
  command_buffer_t value_str;
  common::Error err = GetInner(rkey, &value_str);
  if (err) {
    return err;
  }

  NValue val(common::Value::CreateStringValue(value_str));
  *loaded_key = NDbKValue(key, val);
  return common::Error();
}

common::Error DBConnection::DeleteImpl(const NKeys& keys, NKeys* deleted_keys) {
  for (size_t i = 0; i < keys.size(); ++i) {
    NKey key = keys[i];
    const auto key_str = key.GetKey();
    const raw_key_t rkey = key_str.GetData();

    common::Error err = DelInner(rkey);
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

  const raw_key_t new_rkey = new_key.GetData();
  return SetInner(new_rkey, value_str);
}

common::Error DBConnection::QuitImpl() {
  common::Error err = Disconnect();
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  MDB_dbi ldbi = 0;
  {
    MDB_txn* txn = nullptr;
    common::Error err =
        CheckResultCommand("CONFIG GET DATABASES", mdb_txn_begin(connection_.handle_->env, nullptr, MDB_RDONLY, &txn));
    if (err) {
      return err;
    }

    err = CheckResultCommand("CONFIG GET DATABASES", mdb_dbi_open(txn, nullptr, 0, &ldbi));
    mdb_txn_abort(txn);
    if (err) {
      return err;
    }
  }

  MDB_cursor* cursor = nullptr;
  MDB_txn* txn_dbs = nullptr;
  common::Error err = CheckResultCommand("CONFIG GET DATABASES",
                                         mdb_txn_begin(connection_.handle_->env, nullptr, MDB_RDONLY, &txn_dbs));
  if (err) {
    mdb_dbi_close(connection_.handle_->env, ldbi);
    return err;
  }

  err = CheckResultCommand("CONFIG GET DATABASES", mdb_cursor_open(txn_dbs, ldbi, &cursor));
  if (err) {
    mdb_txn_abort(txn_dbs);
    mdb_dbi_close(connection_.handle_->env, ldbi);
    return err;
  }

  MDB_val key;
  MDB_val data;
  while ((mdb_cursor_get(cursor, &key, &data, MDB_NEXT) == LMDB_OK)) {
    db_name_t skey = GEN_READABLE_STRING_SIZE(reinterpret_cast<const db_name_t::value_type*>(key.mv_data), key.mv_size);
    // std::string sdata(reinterpret_cast<const char*>(data.mv_data),
    // data.mv_size);
    dbs->push_back(skey);
  }

  mdb_cursor_close(cursor);
  mdb_txn_abort(txn_dbs);
  mdb_dbi_close(connection_.handle_->env, ldbi);
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, int err) {
  if (err != LMDB_OK) {
    return GenerateError(cmd, mdb_strerror(err));
  }

  return common::Error();
}

}  // namespace lmdb
}  // namespace core
}  // namespace fastonosql
