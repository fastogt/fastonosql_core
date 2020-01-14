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

#include <fastonosql/core/db/rocksdb/config.h>

#include <common/file_system/types.h>
#include <common/macros.h>

#define ROCKSDB_COMPARATOR_FIELD ARGS_FROM_FIELD("comparator")
#define ROCKSDB_COMPRESSION_FIELD ARGS_FROM_FIELD("compression")
#define ROCKSDB_DB_NAME_FIELD ARGS_FROM_FIELD("n")
#define ROCKSDB_CIM_FIELD ARGS_FROM_FIELD("c")
#define ROCKSDB_MO_FIELD ARGS_FROM_FIELD("mo")

namespace fastonosql {
namespace core {
namespace rocksdb {

const std::vector<const char*> g_comparator_types = {"BYTEWISE", "REVERSE_BYTEWISE"};

const std::vector<const char*> g_compression_types = {
    "NoCompression",  "SnappyCompression", "ZlibCompression",   "BZip2Compression",
    "LZ4Compression", "LZ4HCCompression",  "XpressCompression", "ZSTD"};

const std::vector<const char*> g_merge_operator_types = {
    "None", "Put", "PutV1", "Uint64Add", "StringAppend", "StringAppendIota", "StringAppendTest", "Max", "BytesXor"};

namespace {

const char kDefaultPath[] = "~/test.rocksdb";
const char kDefaultDbName[] = "default";

}  // namespace

Config::Config()
    : LocalConfig(common::file_system::prepare_path(kDefaultPath)),
      create_if_missing(true),
      db_name(kDefaultDbName),
      comparator(COMP_BYTEWISE),
      compression(kNoCompression),
      merge_operator(kNone) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == ROCKSDB_DB_NAME_FIELD && !lastarg) {
      db_name = args[++i];
    } else if (args[i] == ROCKSDB_COMPARATOR_FIELD && !lastarg) {
      ComparatorType lcomparator;
      if (common::ConvertFromString(args[++i], &lcomparator)) {
        comparator = lcomparator;
      }
    } else if (args[i] == ROCKSDB_COMPRESSION_FIELD && !lastarg) {
      CompressionType lcompression;
      if (common::ConvertFromString(args[++i], &lcompression)) {
        compression = lcompression;
      }
    } else if (args[i] == ROCKSDB_CIM_FIELD) {
      create_if_missing = true;
    } else if (args[i] == ROCKSDB_MO_FIELD) {
      MergeOperatorType lmerge_operator;
      if (common::ConvertFromString(args[++i], &lmerge_operator)) {
        merge_operator = lmerge_operator;
      }
    }
  }
}

config_args_t Config::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (create_if_missing) {
    args.push_back(ROCKSDB_CIM_FIELD);
  }

  if (!db_name.empty()) {
    args.push_back(ROCKSDB_DB_NAME_FIELD);
    args.push_back(db_name);
  }

  args.push_back(ROCKSDB_COMPARATOR_FIELD);
  args.push_back(common::ConvertToString(comparator));

  args.push_back(ROCKSDB_COMPRESSION_FIELD);
  args.push_back(common::ConvertToString(compression));

  args.push_back(ROCKSDB_MO_FIELD);
  args.push_back(common::ConvertToString(merge_operator));

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && create_if_missing == other.create_if_missing && db_name == other.db_name &&
         comparator == other.comparator && compression == other.compression && merge_operator == other.merge_operator;
}

}  // namespace rocksdb
}  // namespace core
}  // namespace fastonosql

namespace common {

std::string ConvertToString(fastonosql::core::rocksdb::ComparatorType comp) {
  return fastonosql::core::rocksdb::g_comparator_types[comp];
}

bool ConvertFromString(const std::string& from, fastonosql::core::rocksdb::ComparatorType* out) {
  if (!out || from.empty()) {
    return false;
  }

  for (size_t i = 0; i < fastonosql::core::rocksdb::g_comparator_types.size(); ++i) {
    if (from == fastonosql::core::rocksdb::g_comparator_types[i]) {
      *out = static_cast<fastonosql::core::rocksdb::ComparatorType>(i);
      return true;
    }
  }

  NOTREACHED();
  return false;
}

std::string ConvertToString(fastonosql::core::rocksdb::CompressionType comp) {
  return fastonosql::core::rocksdb::g_compression_types[comp];
}

bool ConvertFromString(const std::string& from, fastonosql::core::rocksdb::CompressionType* out) {
  if (!out || from.empty()) {
    return false;
  }

  for (size_t i = 0; i < fastonosql::core::rocksdb::g_compression_types.size(); ++i) {
    if (from == fastonosql::core::rocksdb::g_compression_types[i]) {
      *out = static_cast<fastonosql::core::rocksdb::CompressionType>(i);
      return true;
    }
  }

  NOTREACHED();
  return false;
}

std::string ConvertToString(fastonosql::core::rocksdb::MergeOperatorType mo) {
  return fastonosql::core::rocksdb::g_merge_operator_types[mo];
}

bool ConvertFromString(const std::string& from, fastonosql::core::rocksdb::MergeOperatorType* out) {
  if (!out || from.empty()) {
    return false;
  }

  for (size_t i = 0; i < fastonosql::core::rocksdb::g_compression_types.size(); ++i) {
    if (from == fastonosql::core::rocksdb::g_merge_operator_types[i]) {
      *out = static_cast<fastonosql::core::rocksdb::MergeOperatorType>(i);
      return true;
    }
  }

  NOTREACHED();
  return false;
}

}  // namespace common
