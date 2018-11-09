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

#include <fastonosql/core/db/leveldb/db_connection.h>

#include <leveldb/c.h>  // for leveldb_major_version, etc
#include <leveldb/db.h>

#include <common/convert2string.h>
#include <common/file_system/string_path_utils.h>

#include <fastonosql/core/db/leveldb/command_translator.h>
#include <fastonosql/core/db/leveldb/comparators/indexed_db.h>
#include <fastonosql/core/db/leveldb/database_info.h>

#include "core/db/leveldb/internal/commands_api.h"

#define LEVELDB_HEADER_STATS                             \
  "                               Compactions\n"         \
  "Level  Files Size(MB) Time(sec) Read(MB) Write(MB)\n" \
  "--------------------------------------------------\n"

namespace fastonosql {
namespace core {
namespace leveldb {
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
}  // namespace
}  // namespace leveldb
template <>
const char* ConnectionTraits<LEVELDB>::GetBasedOn() {
  return "leveldb";
}

template <>
const char* ConnectionTraits<LEVELDB>::GetVersionApi() {
  static std::string leveldb_version = common::MemSPrintf("%d.%d", leveldb_major_version(), leveldb_minor_version());
  return leveldb_version.c_str();
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<LEVELDB>::GetCommands() {
  return leveldb::kCommands;
}
namespace internal {
template <>
common::Error ConnectionAllocatorTraits<leveldb::NativeConnection, leveldb::Config>::Connect(
    const leveldb::Config& config,
    leveldb::NativeConnection** hout) {
  leveldb::NativeConnection* context = nullptr;
  common::Error err = leveldb::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error ConnectionAllocatorTraits<leveldb::NativeConnection, leveldb::Config>::Disconnect(
    leveldb::NativeConnection** handle) {
  destroy(handle);
  return common::Error();
}

template <>
bool ConnectionAllocatorTraits<leveldb::NativeConnection, leveldb::Config>::IsConnected(
    leveldb::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}
}  // namespace internal
namespace leveldb {

common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  ::leveldb::DB* lcontext = nullptr;
  std::string folder = config.db_path;  // start point must be folder
  common::tribool is_dir = common::file_system::is_directory(folder);
  if (is_dir != common::SUCCESS && !config.create_if_missing) {
    return common::make_error(common::MemSPrintf("Invalid input path(%s)", folder));
  }

  ::leveldb::Options lv;
  lv.create_if_missing = config.create_if_missing;
  if (config.comparator == COMP_BYTEWISE) {
    lv.comparator = ::leveldb::BytewiseComparator();
  } else if (config.comparator == COMP_INDEXED_DB) {
    lv.comparator = new comparator::IndexedDB;
  }
  if (config.compression == kNoCompression) {
    lv.compression = ::leveldb::kNoCompression;
  } else if (config.compression == kSnappyCompression) {
    lv.compression = ::leveldb::kSnappyCompression;
  }

  auto st = ::leveldb::DB::Open(lv, folder, &lcontext);
  if (!st.ok()) {
    std::string buff = common::MemSPrintf("Fail connect to server: %s!", st.ToString());
    return common::make_error(buff);
  }

  *context = lcontext;
  return common::Error();
}

common::Error TestConnection(const Config& config) {
  leveldb::NativeConnection* ldb = nullptr;
  common::Error err = CreateConnection(config, &ldb);
  if (err) {
    return err;
  }

  delete ldb;
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())) {}

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

  std::string rets;
  bool isok = connection_.handle_->GetProperty("leveldb.stats", &rets);
  if (!isok) {
    return common::make_error("info function failed");
  }

  ServerInfo::Stats lstats;
  if (rets.size() > sizeof(LEVELDB_HEADER_STATS)) {
    std::vector<std::string> tokens;
    size_t tokens_count = common::Tokenize(rets.c_str() + sizeof(LEVELDB_HEADER_STATS), " \n", &tokens);
    if (tokens_count == 6) {
      uint32_t level;
      if (common::ConvertFromString(tokens[0], &level)) {
        lstats.level = level;
      }

      uint32_t files;
      if (common::ConvertFromString(tokens[1], &files)) {
        lstats.files = files;
      }

      uint32_t size_mb;
      if (common::ConvertFromString(tokens[2], &size_mb)) {
        lstats.size_mb = size_mb;
      }

      uint32_t time_sec;
      if (common::ConvertFromString(tokens[3], &time_sec)) {
        lstats.time_sec = time_sec;
      }

      uint32_t read_mb;
      if (common::ConvertFromString(tokens[4], &read_mb)) {
        lstats.read_mb = read_mb;
      }

      uint32_t write_mb;
      if (common::ConvertFromString(tokens[5], &write_mb)) {
        lstats.read_mb = write_mb;
      }
    }
  }

  *statsout = lstats;
  return common::Error();
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  raw_value_t exist_key;
  common::Error err = GetInner(key, &exist_key);
  if (err) {
    return err;
  }

  const ::leveldb::Slice key_slice(key.data(), key.size());
  ::leveldb::WriteOptions wo;
  return CheckResultCommand(DB_DELETE_KEY_COMMAND, connection_.handle_->Delete(wo, key_slice));
}

common::Error DBConnection::SetInner(const raw_key_t& key, const raw_value_t& value) {
  const ::leveldb::Slice key_slice(key.data(), key.size());
  const ::leveldb::Slice value_slice(value.data(), value.size());
  ::leveldb::WriteOptions wo;
  return CheckResultCommand(DB_SET_KEY_COMMAND, connection_.handle_->Put(wo, key_slice, value_slice));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  const ::leveldb::Slice key_slice(key.data(), key.size());
  ::leveldb::ReadOptions ro;
  std::string ret;
  common::Error err = CheckResultCommand(DB_GET_KEY_COMMAND, connection_.handle_->Get(ro, key_slice, &ret));
  if (err) {
    return err;
  }

  *ret_val = common::ConvertToCharBytes(ret);  // convert from std::string to char bytes
  return common::Error();
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  ::leveldb::ReadOptions ro;
  ::leveldb::Iterator* it = connection_.handle_->NewIterator(ro);
  uint64_t offset_pos = cursor_in;
  cursor_t lcursor_out = 0;
  raw_keys_t lkeys_out;
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    const ::leveldb::Slice key_slice = it->key();
    raw_key_t key = GEN_CMD_STRING_SIZE(key_slice.data(), key_slice.size());
    if (lkeys_out.size() < count_keys) {
      if (common::MatchPattern(key, pattern)) {
        if (offset_pos == 0) {
          lkeys_out.push_back(key);
        } else {
          offset_pos--;
        }
      }
    } else {
      lcursor_out = cursor_in + count_keys;
      break;
    }
  }

  auto st = it->status();
  delete it;

  common::Error err = CheckResultCommand(DB_SCAN_COMMAND, st);
  if (err) {
    return err;
  }

  *keys_out = lkeys_out;
  *cursor_out = lcursor_out;
  return common::Error();
}

common::Error DBConnection::KeysImpl(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* ret) {
  ::leveldb::Slice key_start_slice(key_start.data(), key_start.size());
  ::leveldb::ReadOptions ro;
  ::leveldb::Iterator* it = connection_.handle_->NewIterator(ro);  // keys(key_start, key_end, limit, ret);
  for (it->Seek(key_start_slice); it->Valid(); it->Next()) {
    auto slice = it->key();
    raw_key_t key = GEN_CMD_STRING_SIZE(slice.data(), slice.size());
    if (ret->size() < limit) {
      if (key < key_end) {
        ret->push_back(key);
      }
    } else {
      break;
    }
  }

  auto st = it->status();
  delete it;

  return CheckResultCommand(DB_KEYS_COMMAND, st);
}

common::Error DBConnection::DBkcountImpl(keys_limit_t* size) {
  ::leveldb::ReadOptions ro;
  ::leveldb::Iterator* it = connection_.handle_->NewIterator(ro);
  keys_limit_t sz = 0;
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    sz++;
  }

  auto st = it->status();
  delete it;

  common::Error err = CheckResultCommand(DB_DBKCOUNT_COMMAND, st);
  if (err) {
    return err;
  }

  *size = sz;
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  ::leveldb::ReadOptions ro;
  ::leveldb::WriteOptions wo;
  ::leveldb::Iterator* it = connection_.handle_->NewIterator(ro);
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    std::string key = it->key().ToString();
    common::Error err = CheckResultCommand(DB_FLUSHDB_COMMAND, connection_.handle_->Delete(wo, key));
    if (err) {
      delete it;
      return err;
    }
  }

  auto st = it->status();
  delete it;

  return CheckResultCommand(DB_FLUSHDB_COMMAND, st);
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  if (name != GetCurrentDBName()) {
    return ICommandTranslator::InvalidInputArguments(GEN_CMD_STRING(DB_SELECTDB_COMMAND));
  }

  keys_limit_t kcount = 0;
  common::Error err = DBkcount(&kcount);
  DCHECK(!err) << err->GetDescription();
  *info = new DataBaseInfo(name, true, kcount);
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
  db_names_t ldbs = {GetCurrentDBName()};
  *dbs = ldbs;
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, const ::leveldb::Status& err) {
  if (!err.ok()) {
    return GenerateError(cmd, err.ToString());
  }

  return common::Error();
}

}  // namespace leveldb
}  // namespace core
}  // namespace fastonosql
