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

#include <fastonosql/core/db/rocksdb/db_connection.h>

#include <common/convert2string.h>
#include <common/file_system/string_path_utils.h>

#include <rocksdb/db.h>

#include <fastonosql/core/db/rocksdb/command_translator.h>
#include <fastonosql/core/db/rocksdb/database_info.h>
#include "core/db/rocksdb/internal/commands_api.h"

#define ROCKSDB_HEADER_STATS                                                                                           \
  "\n** Compaction Stats [default] **\nLevel    Files   Size     Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) "  \
  "Moved(GB) W-Amp Rd(MB/s) Wr(MB/s) Comp(sec) Comp(cnt) Avg(sec) KeyIn "                                              \
  "KeyDrop\n---------------------------------------------------------------------------------------------------------" \
  "-------------------------------------------------\n"

#define ROCKSDB_KEYS_COUNT_PROPERTY "rocksdb.estimate-num-keys"
#define ROCKSDB_STATS_PROPERTY "rocksdb.stats"

// hacked
namespace rocksdb {

class MergeOperators {
 public:
  static std::shared_ptr<MergeOperator> CreatePutOperator();
  static std::shared_ptr<MergeOperator> CreateDeprecatedPutOperator();
  static std::shared_ptr<MergeOperator> CreateUInt64AddOperator();
  static std::shared_ptr<MergeOperator> CreateStringAppendOperator();
  static std::shared_ptr<MergeOperator> CreateStringAppendOperator(char delim_char);
  static std::shared_ptr<MergeOperator> CreateStringAppendTESTOperator();
  static std::shared_ptr<MergeOperator> CreateMaxOperator();
  static std::shared_ptr<MergeOperator> CreateBytesXOROperator();
};

}  // namespace rocksdb

namespace fastonosql {
namespace core {

namespace rocksdb {
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
                                         CommandHolder(GEN_CMD_STRING("MGET"),
                                                       "<key> [key ...]",
                                                       "Get the value of a key.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Mget),
                                         CommandHolder(GEN_CMD_STRING("MERGE"),
                                                       "<key> <value>",
                                                       "Merge the database entry for \"key\" "
                                                       "with \"value\"",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Merge),
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
                                                       &CommandsApi::Quit),
                                         CommandHolder(GEN_CMD_STRING(DB_REMOVEDB_COMMAND),
                                                       "<name>",
                                                       "Remove database",
                                                       UNDEFINED_SINCE,
                                                       DB_REMOVEDB_COMMAND " test",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::RemoveDatabase)};
}  // namespace
}  // namespace rocksdb

template <>
const char* ConnectionTraits<ROCKSDB>::GetBasedOn() {
  return "rocksdb";
}

template <>
const char* ConnectionTraits<ROCKSDB>::GetVersionApi() {
  return STRINGIZE(ROCKSDB_MAJOR) "." STRINGIZE(ROCKSDB_MINOR) "." STRINGIZE(ROCKSDB_PATCH);
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<ROCKSDB>::GetCommands() {
  return rocksdb::kCommands;
}

namespace internal {
template <>
common::Error Connection<rocksdb::NativeConnection, rocksdb::Config>::Connect(const rocksdb::Config& config,
                                                                              rocksdb::NativeConnection** hout) {
  rocksdb::NativeConnection* context = nullptr;
  common::Error err = rocksdb::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error Connection<rocksdb::NativeConnection, rocksdb::Config>::Disconnect(rocksdb::NativeConnection** handle) {
  destroy(handle);
  return common::Error();
}

template <>
bool Connection<rocksdb::NativeConnection, rocksdb::Config>::IsConnected(rocksdb::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}

}  // namespace internal
namespace rocksdb {
class rocksdb_handle {
 public:
  rocksdb_handle(::rocksdb::DB* db, std::vector<::rocksdb::ColumnFamilyHandle*> handles)
      : db_(db), handles_(handles), current_db_index_(0) {}
  ~rocksdb_handle() {
    for (auto handle : handles_) {
      delete handle;
    }
    handles_.clear();
    delete db_;
  }

  bool GetProperty(const ::rocksdb::Slice& property, std::string* value) {
    return db_->GetProperty(GetCurrentColumn(), property, value);
  }

  ::rocksdb::Status Get(const ::rocksdb::ReadOptions& options, const ::rocksdb::Slice& key, std::string* value) {
    return db_->Get(options, GetCurrentColumn(), key, value);
  }

  std::vector<::rocksdb::Status> MultiGet(const ::rocksdb::ReadOptions& options,
                                          const std::vector<::rocksdb::Slice>& keys,
                                          std::vector<std::string>* values) {
    return db_->MultiGet(options, handles_, keys, values);
  }

  ::rocksdb::Status Merge(const ::rocksdb::WriteOptions& options,
                          const ::rocksdb::Slice& key,
                          const ::rocksdb::Slice& value) {
    return db_->Merge(options, GetCurrentColumn(), key, value);
  }

  ::rocksdb::Status Put(const ::rocksdb::WriteOptions& options,
                        const ::rocksdb::Slice& key,
                        const ::rocksdb::Slice& value) {
    return db_->Put(options, GetCurrentColumn(), key, value);
  }

  ::rocksdb::Status Delete(const ::rocksdb::WriteOptions& options, const ::rocksdb::Slice& key) {
    return db_->Delete(options, GetCurrentColumn(), key);
  }

  ::rocksdb::Iterator* NewIterator(const ::rocksdb::ReadOptions& options) {
    return db_->NewIterator(options, GetCurrentColumn());
  }

  ::rocksdb::ColumnFamilyHandle* GetCurrentColumn() const { return handles_[current_db_index_]; }
  std::string GetCurrentDBName() const {
    ::rocksdb::ColumnFamilyHandle* fam = GetCurrentColumn();
    return fam->GetName();
  }

  ::rocksdb::Status Select(const std::string& name) {
    if (name.empty()) {  // only for named dbs
      return ::rocksdb::Status::InvalidArgument();
    }

    if (name == GetCurrentDBName()) {  // lazy select
      return ::rocksdb::Status();
    }

    for (size_t i = 0; i < handles_.size(); ++i) {
      ::rocksdb::ColumnFamilyHandle* fam = handles_[i];
      if (fam->GetName() == name) {
        current_db_index_ = i;
        return ::rocksdb::Status();
      }
    }

    return ::rocksdb::Status::NotFound();
  }

  ::rocksdb::Status CreateDB(const std::string& name) {
    if (name.empty()) {  // only for named dbs
      return ::rocksdb::Status::InvalidArgument();
    }

    for (size_t i = 0; i < handles_.size(); ++i) {
      ::rocksdb::ColumnFamilyHandle* fam = handles_[i];
      if (fam->GetName() == name) {
        return ::rocksdb::Status();
      }
    }

    ::rocksdb::ColumnFamilyHandle* fam = nullptr;
    ::rocksdb::Status st = db_->CreateColumnFamily(::rocksdb::ColumnFamilyOptions(), name, &fam);
    if (st.ok()) {
      handles_.push_back(fam);
    }
    return st;
  }

  ::rocksdb::Status RemoveDB(const std::string& name) {
    if (name.empty()) {  // only for named dbs
      return ::rocksdb::Status::InvalidArgument();
    }

    if (name == GetCurrentDBName()) {  // don't want to delete selected db
      return ::rocksdb::Status::InvalidArgument();
    }

    for (size_t i = 0; i < handles_.size(); ++i) {
      ::rocksdb::ColumnFamilyHandle* fam = handles_[i];
      if (fam->GetName() == name) {
        ::rocksdb::Status st = db_->DropColumnFamily(fam);
        if (st.ok()) {
          handles_.erase(handles_.begin() + i);
          if (i < current_db_index_) {
            current_db_index_--;
          }
        }
        return st;
      }
    }

    return ::rocksdb::Status::NotFound();
  }

  db_names_t GetDatabasesNames() const {
    db_names_t res;
    for (size_t i = 0; i < handles_.size(); ++i) {
      ::rocksdb::ColumnFamilyHandle* fam = handles_[i];
      res.push_back(common::ConvertToCharBytes(fam->GetName()));  // convert from std::string to char bytes
    }
    return res;
  }

 private:
  ::rocksdb::DB* db_;
  std::vector<::rocksdb::ColumnFamilyHandle*> handles_;
  size_t current_db_index_;

  DISALLOW_COPY_AND_ASSIGN(rocksdb_handle);
};

common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  std::string folder = config.db_path;  // start point must be folder
  common::tribool is_dir = common::file_system::is_directory(folder);
  if (is_dir != common::SUCCESS && !config.create_if_missing) {
    return common::make_error(common::MemSPrintf("Invalid input path(%s)", folder));
  }

  ::rocksdb::Options rs;
  rs.create_if_missing = config.create_if_missing;
  if (config.comparator == COMP_BYTEWISE) {
    rs.comparator = ::rocksdb::BytewiseComparator();
  } else if (config.comparator == COMP_REVERSE_BYTEWISE) {
    rs.comparator = ::rocksdb::ReverseBytewiseComparator();
  } else {
    NOTREACHED() << "Not handled comparator: " << config.comparator;
  }

  if (config.compression == kNoCompression) {
    rs.compression = ::rocksdb::kNoCompression;
  } else if (config.compression == kSnappyCompression) {
    rs.compression = ::rocksdb::kSnappyCompression;
  } else if (config.compression == kZlibCompression) {
    rs.compression = ::rocksdb::kZlibCompression;
  } else if (config.compression == kBZip2Compression) {
    rs.compression = ::rocksdb::kBZip2Compression;
  } else if (config.compression == kLZ4Compression) {
    rs.compression = ::rocksdb::kLZ4Compression;
  } else if (config.compression == kLZ4HCCompression) {
    rs.compression = ::rocksdb::kLZ4HCCompression;
  } else if (config.compression == kXpressCompression) {
    rs.compression = ::rocksdb::kXpressCompression;
  } else if (config.compression == kZSTD) {
    rs.compression = ::rocksdb::kZSTD;
  } else {
    NOTREACHED() << "Not handled compression: " << config.compression;
  }

  if (config.merge_operator == kNone) {
    rs.merge_operator = nullptr;
  } else if (config.merge_operator == kPut) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreatePutOperator();
  } else if (config.merge_operator == kPutV1) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateDeprecatedPutOperator();
  } else if (config.merge_operator == kUint64Add) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateUInt64AddOperator();
  } else if (config.merge_operator == kStringAppend) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateStringAppendOperator();
  } else if (config.merge_operator == kStringAppendIota) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateStringAppendOperator(',');
  } else if (config.merge_operator == kStringAppendTest) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateStringAppendTESTOperator();
  } else if (config.merge_operator == kMax) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateMaxOperator();
  } else if (config.merge_operator == kBytesXor) {
    rs.merge_operator = ::rocksdb::MergeOperators::CreateBytesXOROperator();
  } else {
    NOTREACHED() << "Not handled merge_operator: " << config.merge_operator;
  }

  std::vector<std::string> column_families_str;
  auto st = ::rocksdb::DB::ListColumnFamilies(rs, folder, &column_families_str);
  if (!st.ok()) {
    if (!rs.create_if_missing) {
      std::string buff = common::MemSPrintf("Fail open database: %s!", st.ToString());
      return common::make_error(buff);
    }
  }

  std::vector<::rocksdb::ColumnFamilyDescriptor> column_families;
  for (size_t i = 0; i < column_families_str.size(); ++i) {
    ::rocksdb::ColumnFamilyDescriptor descr(column_families_str[i], ::rocksdb::ColumnFamilyOptions());
    column_families.push_back(descr);
  }

  if (column_families.empty()) {
    column_families = {::rocksdb::ColumnFamilyDescriptor()};
  }

  ::rocksdb::DB* ldbcontext = nullptr;
  std::vector<::rocksdb::ColumnFamilyHandle*> lhandles;
  st = ::rocksdb::DB::Open(rs, folder, column_families, &lhandles, &ldbcontext);
  if (!st.ok()) {
    std::string buff = common::MemSPrintf("Fail open database: %s!", st.ToString());
    return common::make_error(buff);
  }

  const std::string db_name = config.db_name;
  rocksdb_handle* lcontext = new rocksdb_handle(ldbcontext, lhandles);
  st = lcontext->Select(db_name);
  if (!st.ok()) {
    delete lcontext;
    std::string buff = common::MemSPrintf("Failed select database: %s", st.ToString());
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

  delete ldb;
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())) {}

common::Error DBConnection::Info(std::string* statsout) {
  if (!statsout) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return GetProperty(ROCKSDB_STATS_PROPERTY, statsout);
}

common::Error DBConnection::GetProperty(const std::string& property, std::string* out) {
  if (!out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  std::string rets;
  bool isok = connection_.handle_->GetProperty(property, &rets);
  if (!isok) {
    return common::make_error("GetProperty function failed!");
  }

  *out = rets;
  return common::Error();
}

common::Error DBConnection::Info(ServerInfo::Stats* statsout) {
  if (!statsout) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::string rets;
  err = GetProperty(ROCKSDB_STATS_PROPERTY, &rets);
  if (err) {
    return err;
  }

  ServerInfo::Stats lstats;
  if (rets.size() > sizeof(ROCKSDB_HEADER_STATS)) {
    const std::string part = std::string(rets.c_str() + sizeof(ROCKSDB_HEADER_STATS));
    const size_t delem = part.find_first_of('\n');
    const std::string field = part.substr(0, delem);

    std::vector<std::string> tokens;
    size_t tokens_count = common::Tokenize(field, " ", &tokens);
    if (tokens_count == 19) {
      lstats.level = tokens[0];
      lstats.files = tokens[1];

      double size;
      if (common::ConvertFromString(tokens[2], &size)) {
        lstats.size = size;
      }

      double score;
      if (common::ConvertFromString(tokens[4], &score)) {
        lstats.score = score;
      }

      double read_gb;
      if (common::ConvertFromString(tokens[5], &read_gb)) {
        lstats.read_gb = read_gb;
      }

      double rn_gb;
      if (common::ConvertFromString(tokens[6], &rn_gb)) {
        lstats.rn_gb = rn_gb;
      }

      double rn_p1;
      if (common::ConvertFromString(tokens[7], &rn_p1)) {
        lstats.rn_p1 = rn_p1;
      }

      double write_gb;
      if (common::ConvertFromString(tokens[8], &write_gb)) {
        lstats.write_gb = write_gb;
      }

      double wnew_gb;
      if (common::ConvertFromString(tokens[9], &wnew_gb)) {
        lstats.wnew_gb = wnew_gb;
      }

      double moved_gb;
      if (common::ConvertFromString(tokens[10], &moved_gb)) {
        lstats.moved_gb = moved_gb;
      }

      double wamp;
      if (common::ConvertFromString(tokens[11], &wamp)) {
        lstats.wamp = wamp;
      }

      double rd_mbs;
      if (common::ConvertFromString(tokens[12], &rd_mbs)) {
        lstats.rd_mbs = rd_mbs;
      }

      double wr_mbs;
      if (common::ConvertFromString(tokens[13], &wr_mbs)) {
        lstats.wr_mbs = wr_mbs;
      }

      double comp_sec;
      if (common::ConvertFromString(tokens[14], &comp_sec)) {
        lstats.comp_sec = comp_sec;
      }

      double comp_cnt;
      if (common::ConvertFromString(tokens[15], &comp_cnt)) {
        lstats.comp_cnt = comp_cnt;
      }

      uint32_t avg_sec;
      if (common::ConvertFromString(tokens[16], &avg_sec)) {
        lstats.avg_sec = avg_sec;
      }

      double key_in;
      if (common::ConvertFromString(tokens[17], &key_in)) {
        lstats.key_in = key_in;
      }

      double key_drop;
      if (common::ConvertFromString(tokens[18], &key_drop)) {
        lstats.key_drop = key_drop;
      }
    }
  }

  *statsout = lstats;
  return common::Error();
}

db_name_t DBConnection::GetCurrentDBName() const {
  if (IsConnected()) {
    return common::ConvertToCharBytes(connection_.handle_->GetCurrentDBName());
  }

  DNOTREACHED();
  return base_class::GetCurrentDBName();
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  ::rocksdb::ReadOptions ro;
  const ::rocksdb::Slice key_slice(key.data(), key.size());
  std::string ret;
  common::Error err = CheckResultCommand(DB_GET_KEY_COMMAND, connection_.handle_->Get(ro, key_slice, &ret));
  if (err) {
    return err;
  }

  *ret_val = common::ConvertToCharBytes(ret);
  return common::Error();
}

common::Error DBConnection::Mget(const std::vector<command_buffer_t>& keys, std::vector<command_buffer_t>* ret) {
  if (keys.empty() || !ret) {
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<::rocksdb::Slice> rslice;
  for (auto key : keys) {
    rslice.push_back(common::ConvertToString(key));
  }

  std::vector<std::string> ret_values;
  ::rocksdb::ReadOptions ro;
  auto sts = connection_.handle_->MultiGet(ro, rslice, &ret_values);
  for (size_t i = 0; i < sts.size(); ++i) {
    common::Error err = CheckResultCommand("MGET", sts[i]);
    if (err) {
      return err;
    }

    (*ret).push_back(common::ConvertToCharBytes(ret_values[i]));
  }

  return common::Error();
}

common::Error DBConnection::Merge(const command_buffer_t& key, const command_buffer_t& value) {
  if (key.empty() || value.empty()) {
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  ::rocksdb::WriteOptions wo;
  const std::string key_str = common::ConvertToString(key);
  const std::string value_str = common::ConvertToString(value);
  return CheckResultCommand("MERGE", connection_.handle_->Merge(wo, key_str, value_str));
}

IServerInfo* DBConnection::MakeServerInfo(const std::string& content) const {
  return new ServerInfo(content);
}

IDataBaseInfo* DBConnection::MakeDatabaseInfo(const db_name_t& name, bool is_default, size_t size) const {
  return new DataBaseInfo(name, is_default, size);
}

common::Error DBConnection::SetInner(const raw_key_t& key, const raw_value_t& value) {
  ::rocksdb::WriteOptions wo;
  const ::rocksdb::Slice key_slice(key.data(), key.size());
  const ::rocksdb::Slice value_slice(value.data(), value.size());
  return CheckResultCommand(DB_SET_KEY_COMMAND, connection_.handle_->Put(wo, key_slice, value_slice));
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  command_buffer_t exist_key;
  common::Error err = GetInner(key, &exist_key);
  if (err) {
    return err;
  }

  ::rocksdb::WriteOptions wo;
  const ::rocksdb::Slice key_slice(key.data(), key.size());
  return CheckResultCommand(DB_DELETE_KEY_COMMAND, connection_.handle_->Delete(wo, key_slice));
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  ::rocksdb::ReadOptions ro;
  ::rocksdb::Iterator* it = connection_.handle_->NewIterator(ro);  // keys(key_start, key_end, limit, ret);
  cursor_t offset_pos = cursor_in;
  cursor_t lcursor_out = 0;
  std::vector<command_buffer_t> lkeys_out;
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    const ::rocksdb::Slice key_slice = it->key();
    if (lkeys_out.size() < count_keys) {
      if (IsKeyMatchPattern(key_slice.data(), key_slice.size(), pattern)) {
        if (offset_pos == 0) {
          const raw_key_t key = GEN_CMD_STRING_SIZE(key_slice.data(), key_slice.size());
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
  const ::rocksdb::Slice key_start_slice(key_start.data(), key_start.size());
  ::rocksdb::ReadOptions ro;
  ::rocksdb::Iterator* it = connection_.handle_->NewIterator(ro);  // keys(key_start, key_end, limit, ret);
  for (it->Seek(key_start_slice); it->Valid(); it->Next()) {
    auto slice = it->key();
    command_buffer_t key = GEN_CMD_STRING_SIZE(slice.data(), slice.size());
    if (ret->size() < limit) {
      if (IsKeyInEndRange(key, key_end)) {
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

common::Error DBConnection::DBKeysCountImpl(keys_limit_t* size) {
  keys_limit_t sz = 0;
  common::Error err;
#if 0
  std::string ret;
  err = GetProperty(ROCKSDB_KEYS_COUNT_PROPERTY, &ret);
  if (!err) {
    if (common::ConvertFromString(ret, &sz)) {
      *size = sz;
      return common::Error();
    }
  }
#endif

  // old calc
  ::rocksdb::ReadOptions ro;
  ::rocksdb::Iterator* it = connection_.handle_->NewIterator(ro);
  for (it->SeekToFirst(); it->Valid(); it->Next()) {
    sz++;
  }

  auto st = it->status();
  delete it;

  err = CheckResultCommand(DB_DBKCOUNT_COMMAND, st);
  if (err) {
    return err;
  }

  *size = sz;
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  ::rocksdb::ReadOptions ro;
  ::rocksdb::WriteOptions wo;
  ::rocksdb::Iterator* it = connection_.handle_->NewIterator(ro);
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

common::Error DBConnection::CreateDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  const std::string db_name = common::ConvertToString(name);
  common::Error err = CheckResultCommand(DB_CREATEDB_COMMAND, connection_.handle_->CreateDB(db_name));
  if (err) {
    return err;
  }

  *info = new DataBaseInfo(name, false, 0);
  return common::Error();
}

common::Error DBConnection::RemoveDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  const std::string db_name = common::ConvertToString(name);
  common::Error err = CheckResultCommand(DB_REMOVEDB_COMMAND, connection_.handle_->RemoveDB(db_name));
  if (err) {
    return err;
  }

  *info = new DataBaseInfo(name, false, 0);
  return common::Error();
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  const std::string db_name = common::ConvertToString(name);
  common::Error err = CheckResultCommand(DB_SELECTDB_COMMAND, connection_.handle_->Select(db_name));
  if (err) {
    return err;
  }

  keys_limit_t kcount = 0;
  err = DBKeysCount(&kcount);
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
    const NKey key = keys[i];
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
  return Disconnect();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  *dbs = connection_.handle_->GetDatabasesNames();
  return common::Error();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, const ::rocksdb::Status& err) {
  if (!err.ok()) {
    return GenerateError(cmd, err.ToString());
  }

  return common::Error();
}

}  // namespace rocksdb
}  // namespace core
}  // namespace fastonosql
