/*  Copyright (C) 2014-2017 FastoGT. All right reserved.

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

#include <fastonosql/core/db/upscaledb/db_connection.h>

#include <ups/upscaledb.h>

#include <common/convert2string.h>
#include <common/file_system/string_path_utils.h>

#include <fastonosql/core/db/upscaledb/command_translator.h>
#include <fastonosql/core/db/upscaledb/database_info.h>
#include "core/db/upscaledb/internal/commands_api.h"

namespace fastonosql {
namespace core {
namespace {
ups_key_t ConvertToUpscaleDBSlice(const command_buffer_t& key) {
  ups_key_t dkey;
  memset(&dkey, 0, sizeof(dkey));
  dkey.size = key.size();
  dkey.data = const_cast<readable_string_t::value_type*>(key.data());
  return dkey;
}
}  // namespace
namespace upscaledb {

struct upscaledb {
  ups_env_t* env;
  ups_db_t* db;
  uint16_t cur_db;
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
                                                       DB_INFO_COMMAND " stats",
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
                                                       DB_SCAN_COMMAND " MATCH * COUNT 10",
                                                       1,
                                                       4,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Scan),
                                         CommandHolder(GEN_CMD_STRING(DB_JSONDUMP_COMMAND),
                                                       "<cursor> PATH absolute_path [MATCH pattern] [COUNT count]",
                                                       "Dump DB into json file by path.",
                                                       UNDEFINED_SINCE,
                                                       DB_JSONDUMP_COMMAND " PATH ~/dump.json MATCH * COUNT 10",
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
                                         CommandHolder(GEN_CMD_STRING(DB_SELECTDB_COMMAND),
                                                       "<name>",
                                                       "Change the selected database for the "
                                                       "current connection",
                                                       UNDEFINED_SINCE,
                                                       DB_SELECTDB_COMMAND " test",
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
                                                       DB_GET_KEY_COMMAND " get",
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

int upscaledb_select(upscaledb* context, uint16_t num) {
  if (!context) {
    return EINVAL;
  }

  if (context->cur_db == num) {  // lazy select
    return UPS_SUCCESS;
  }

  ups_db_t* db = NULL;
  ups_status_t st = ups_env_open_db(context->env, &db, num, 0, NULL);
  if (st != UPS_SUCCESS) {
    return st;
  }

  st = ups_db_close(context->db, 0);
  DCHECK(st == UPS_SUCCESS);
  context->db = db;
  context->cur_db = num;
  return UPS_SUCCESS;
}

ups_status_t upscaledb_open(upscaledb** context, const char* dbpath, uint16_t db, bool create_if_missing) {
  upscaledb* lcontext = reinterpret_cast<upscaledb*>(calloc(1, sizeof(upscaledb)));
  bool need_to_create = false;
  if (create_if_missing) {
    bool exist = common::file_system::is_file_exist(std::string(dbpath));
    if (!exist) {
      need_to_create = true;
    }
  }

  ups_status_t st =
      need_to_create ? ups_env_create(&lcontext->env, dbpath, 0, 0664, 0) : ups_env_open(&lcontext->env, dbpath, 0, 0);
  if (st != UPS_SUCCESS) {
    free(lcontext);
    return st;
  }

  st = need_to_create ? ups_env_create_db(lcontext->env, &lcontext->db, db, 0, NULL)
                      : ups_env_open_db(lcontext->env, &lcontext->db, db, 0, NULL);
  if (st != UPS_SUCCESS) {
    free(lcontext);
    return st;
  }

  lcontext->cur_db = db;
  *context = lcontext;
  return UPS_SUCCESS;
}

void upscaledb_close(upscaledb** context) {
  if (!context) {
    return;
  }

  upscaledb* lcontext = *context;
  if (!lcontext) {
    return;
  }

  ups_status_t st = ups_db_close(lcontext->db, 0);
  DCHECK(st == UPS_SUCCESS);
  st = ups_env_close(lcontext->env, 0);
  DCHECK(st == UPS_SUCCESS);
  free(lcontext);
  *context = NULL;
}
}  // namespace
}  // namespace upscaledb
template <>
const char* ConnectionTraits<UPSCALEDB>::GetBasedOn() {
  return "libupscaledb";
}

template <>
const char* ConnectionTraits<UPSCALEDB>::GetVersionApi() {
  return STRINGIZE(UPS_VERSION_MAJ) "." STRINGIZE(UPS_VERSION_MIN) "." STRINGIZE(UPS_VERSION_REV);
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<UPSCALEDB>::GetCommands() {
  return upscaledb::kCommands;
}
namespace internal {
template <>
common::Error ConnectionAllocatorTraits<upscaledb::NativeConnection, upscaledb::Config>::Connect(
    const upscaledb::Config& config,
    upscaledb::NativeConnection** hout) {
  upscaledb::NativeConnection* context = nullptr;
  common::Error err = upscaledb::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error ConnectionAllocatorTraits<upscaledb::NativeConnection, upscaledb::Config>::Disconnect(
    upscaledb::NativeConnection** handle) {
  upscaledb::upscaledb_close(handle);
  *handle = nullptr;
  return common::Error();
}

template <>
bool ConnectionAllocatorTraits<upscaledb::NativeConnection, upscaledb::Config>::IsConnected(
    upscaledb::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}

}  // namespace internal
namespace upscaledb {
common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  DCHECK(*context == NULL);
  struct upscaledb* lcontext = NULL;
  std::string db_path = config.db_path;  // start point must be folder
  std::string folder = common::file_system::get_dir_path(db_path);
  common::tribool is_dir = common::file_system::is_directory(folder);
  if (is_dir == common::INDETERMINATE) {
    return common::make_error(common::MemSPrintf("Invalid input path(%s)", folder));
  }

  if (is_dir != common::SUCCESS) {
    return common::make_error(common::MemSPrintf("Invalid input path(%s)", db_path));
  }

  const char* dbname = db_path.empty() ? NULL : db_path.c_str();
  int st = upscaledb_open(&lcontext, dbname, config.dbnum, config.create_if_missing);
  if (st != UPS_SUCCESS) {
    std::string buff = common::MemSPrintf("Fail open database: %s", ups_strerror(st));
    return common::make_error(buff);
  }

  *context = lcontext;
  return common::Error();
}

common::Error TestConnection(const Config& config) {
  struct upscaledb* scaledb = NULL;
  common::Error err = CreateConnection(config, &scaledb);
  if (err) {
    return err;
  }

  upscaledb_close(&scaledb);
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())) {}

db_name_t DBConnection::GetCurrentDBName() const {
  if (connection_.handle_) {
    return common::ConvertToCharBytes(connection_.handle_->cur_db);
  }

  DNOTREACHED();
  return base_class::GetCurrentDBName();
}

common::Error DBConnection::Connect(const config_t& config) {
  common::Error err = base_class::Connect(config);
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::Disconnect() {
  connection_.handle_->cur_db = 0;
  return base_class::Disconnect();
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

common::Error DBConnection::SetInner(const raw_key_t& key, const raw_value_t& value) {
  ups_key_t key_slice = ConvertToUpscaleDBSlice(key);
  ups_record_t rec;
  memset(&rec, 0, sizeof(rec));
  rec.data = const_cast<command_buffer_char_t*>(value.data());
  rec.size = value.size();
  return CheckResultCommand(DB_SET_KEY_COMMAND,
                            ups_db_insert(connection_.handle_->db, 0, &key_slice, &rec, UPS_OVERWRITE));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  ups_key_t key_slice = ConvertToUpscaleDBSlice(key);

  ups_record_t rec;
  memset(&rec, 0, sizeof(rec));

  common::Error err =
      CheckResultCommand(DB_GET_KEY_COMMAND, ups_db_find(connection_.handle_->db, NULL, &key_slice, &rec, 0));
  if (err) {
    return err;
  }

  *ret_val = GEN_CMD_STRING_SIZE(reinterpret_cast<const command_buffer_char_t*>(rec.data), rec.size);
  return common::Error();
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  ups_key_t key_slice = ConvertToUpscaleDBSlice(key);
  return CheckResultCommand(DB_DELETE_KEY_COMMAND, ups_db_erase(connection_.handle_->db, NULL, &key_slice, 0));
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  ups_cursor_t* cursor; /* upscaledb cursor object */
  ups_key_t key;
  ups_record_t rec;

  memset(&key, 0, sizeof(key));
  memset(&rec, 0, sizeof(rec));

  /* create a new cursor */
  common::Error err = CheckResultCommand(DB_SCAN_COMMAND, ups_cursor_create(&cursor, connection_.handle_->db, 0, 0));
  if (err) {
    return err;
  }

  ups_status_t st = UPS_SUCCESS;
  uint64_t offset_pos = cursor_in;
  uint64_t lcursor_out = 0;
  std::vector<command_buffer_t> lkeys_out;
  while (st == UPS_SUCCESS) {
    if (lkeys_out.size() < count_keys) {
      /* fetch the next item, and repeat till we've reached the end
       * of the database */
      st = ups_cursor_move(cursor, &key, &rec, UPS_CURSOR_NEXT | UPS_SKIP_DUPLICATES);
      if (st == UPS_SUCCESS) {
        command_buffer_t skey = GEN_CMD_STRING_SIZE(reinterpret_cast<const command_buffer_char_t*>(key.data), key.size);
        if (common::MatchPattern(skey, pattern)) {
          if (offset_pos == 0) {
            lkeys_out.push_back(skey);
          } else {
            offset_pos--;
          }
        }
      } else if (st != UPS_KEY_NOT_FOUND) {
        ups_cursor_close(cursor);
        std::string buff = common::MemSPrintf("SCAN function error: %s", ups_strerror(st));
        return common::make_error(buff);
      }
    } else {
      lcursor_out = cursor_in + count_keys;
      break;
    }
  }

  ups_cursor_close(cursor);
  *keys_out = lkeys_out;
  *cursor_out = lcursor_out;
  return common::Error();
}

common::Error DBConnection::KeysImpl(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* ret) {
  ups_cursor_t* cursor; /* upscaledb cursor object */
  ups_key_t key;
  ups_record_t rec;

  memset(&key, 0, sizeof(key));
  memset(&rec, 0, sizeof(rec));

  /* create a new cursor */
  common::Error err = CheckResultCommand(DB_KEYS_COMMAND, ups_cursor_create(&cursor, connection_.handle_->db, 0, 0));
  if (err) {
    return err;
  }

  ups_status_t st;
  do {
    st = ups_cursor_move(cursor, &key, &rec, UPS_CURSOR_NEXT | UPS_SKIP_DUPLICATES);
    if (st == UPS_SUCCESS) {
      command_buffer_t skey = GEN_CMD_STRING_SIZE(reinterpret_cast<const command_buffer_char_t*>(key.data), key.size);
      if (key_start < skey && key_end > skey) {
        ret->push_back(skey);
      }
    } else if (st != UPS_KEY_NOT_FOUND) {
      ups_cursor_close(cursor);
      std::string buff = common::MemSPrintf("KEYS function error: %s", ups_strerror(st));
      return common::make_error(buff);
    }
  } while (st == UPS_SUCCESS && limit > ret->size());

  ups_cursor_close(cursor);
  return common::Error();
}

common::Error DBConnection::DBkcountImpl(keys_limit_t* size) {
  uint64_t sz = 0;
  common::Error err =
      CheckResultCommand(DB_DBKCOUNT_COMMAND, ups_db_count(connection_.handle_->db, NULL, UPS_SKIP_DUPLICATES, &sz));
  if (err) {
    return err;
  }

  *size = sz;
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  ups_cursor_t* cursor; /* upscaledb cursor object */
  ups_key_t key;
  ups_record_t rec;

  memset(&key, 0, sizeof(key));
  memset(&rec, 0, sizeof(rec));

  /* create a new cursor */
  common::Error err = CheckResultCommand(DB_FLUSHDB_COMMAND, ups_cursor_create(&cursor, connection_.handle_->db, 0, 0));
  if (err) {
    return err;
  }

  ups_status_t st;
  do {
    /* fetch the next item, and repeat till we've reached the end
     * of the database */
    st = ups_cursor_move(cursor, &key, &rec, UPS_CURSOR_NEXT);
    if (st == UPS_SUCCESS) {
      ups_db_erase(connection_.handle_->db, 0, &key, 0);
    } else if (st && st != UPS_KEY_NOT_FOUND) {
      ups_cursor_close(cursor);
      std::string buff = common::MemSPrintf("FLUSHDB function error: %s", ups_strerror(st));
      return common::make_error(buff);
    }
  } while (st == UPS_SUCCESS);

  ups_cursor_close(cursor);
  return common::Error();
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  uint16_t num;
  if (!common::ConvertFromBytes(name, &num)) {
    return common::make_error_inval();
  }

  ups_status_t st = upscaledb_select(connection_.handle_, num);
  if (st != UPS_SUCCESS) {
    std::string buff = common::MemSPrintf("SELECT function error: %s", ups_strerror(st));
    return common::make_error(buff);
  }

  keys_limit_t kcount = 0;
  common::Error err = DBkcount(&kcount);
  DCHECK(!err);
  *info = new DataBaseInfo(name, true, kcount);
  return common::Error();
}

common::Error DBConnection::SetImpl(const NDbKValue& key) {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();
  const auto value_str = value.GetReadableValue();

  common::Error err = SetInner(key_str.GetData(), value_str.GetData());
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::GetImpl(const NKey& key, NDbKValue* loaded_key) {
  const key_t key_str = key.GetKey();
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
    const NKey key = keys[i];
    const key_t key_str = key.GetKey();
    const raw_key_t rkey = key_str.GetData();

    common::Error err = DelInner(rkey);
    if (err) {
      continue;
    }

    deleted_keys->push_back(key);
  }

  return common::Error();
}

common::Error DBConnection::RenameImpl(const NKey& key, const key_t& new_key) {
  const key_t key_str = key.GetKey();
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

common::Error DBConnection::QuitImpl() {
  common::Error err = Disconnect();
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  db_names_t ldbs = {GetCurrentDBName()};
  *dbs = ldbs;
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, ups_status_t err) {
  if (err != UPS_SUCCESS) {
    return GenerateError(cmd, ups_strerror(err));
  }

  return common::Error();
}

}  // namespace upscaledb
}  // namespace core
}  // namespace fastonosql
