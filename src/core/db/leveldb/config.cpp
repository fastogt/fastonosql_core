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

#include <fastonosql/core/db/leveldb/config.h>

#include <common/file_system/types.h>  // for prepare_path

#include <leveldb/options.h>  // for Options

#define LEVELDB_COMPARATOR_FIELD ARGS_FROM_FIELD("comparator")
#define LEVELDB_COMPRESSION_FIELD ARGS_FROM_FIELD("compression")
#define LEVELDB_CIM_FIELD ARGS_FROM_FIELD("c")

namespace fastonosql {
namespace core {
namespace leveldb {

const std::vector<const char*> g_comparator_types = {"BYTEWISE", "INDEXED_DB"};

const std::vector<const char*> g_compression_types = {"NO_COMPRESSION", "SNAPPY"};

namespace {

const char kDefaultPath[] = "~/test.leveldb";

}  // namespace

Config::Config()
    : base_class(common::file_system::prepare_path(kDefaultPath)),
      create_if_missing(true),
      comparator(COMP_BYTEWISE),
      compression(kNoCompression) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == LEVELDB_COMPARATOR_FIELD && !lastarg) {
      ComparatorType lcomparator;
      if (common::ConvertFromString(args[++i], &lcomparator)) {
        comparator = lcomparator;
      }
    } else if (args[i] == LEVELDB_COMPRESSION_FIELD && !lastarg) {
      CompressionType lcompression;
      if (common::ConvertFromString(args[++i], &lcompression)) {
        compression = lcompression;
      }
    } else if (args[i] == LEVELDB_CIM_FIELD) {
      create_if_missing = true;
    }
  }
}

config_args_t Config::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (create_if_missing) {
    args.push_back(LEVELDB_CIM_FIELD);
  }

  args.push_back(LEVELDB_COMPARATOR_FIELD);
  args.push_back(common::ConvertToString(comparator));

  args.push_back(LEVELDB_COMPRESSION_FIELD);
  args.push_back(common::ConvertToString(compression));

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && create_if_missing == other.create_if_missing && comparator == other.comparator &&
         compression == other.compression;
}

}  // namespace leveldb
}  // namespace core
}  // namespace fastonosql

namespace common {

std::string ConvertToString(fastonosql::core::leveldb::ComparatorType comp) {
  return fastonosql::core::leveldb::g_comparator_types[comp];
}

bool ConvertFromString(const std::string& from, fastonosql::core::leveldb::ComparatorType* out) {
  if (!out || from.empty()) {
    return false;
  }

  for (size_t i = 0; i < fastonosql::core::leveldb::g_comparator_types.size(); ++i) {
    if (from == fastonosql::core::leveldb::g_comparator_types[i]) {
      *out = static_cast<fastonosql::core::leveldb::ComparatorType>(i);
      return true;
    }
  }

  NOTREACHED();
  return false;
}

std::string ConvertToString(fastonosql::core::leveldb::CompressionType comp) {
  return fastonosql::core::leveldb::g_compression_types[comp];
}

bool ConvertFromString(const std::string& from, fastonosql::core::leveldb::CompressionType* out) {
  if (!out || from.empty()) {
    return false;
  }

  for (size_t i = 0; i < fastonosql::core::leveldb::g_compression_types.size(); ++i) {
    if (from == fastonosql::core::leveldb::g_compression_types[i]) {
      *out = static_cast<fastonosql::core::leveldb::CompressionType>(i);
      return true;
    }
  }

  NOTREACHED();
  return false;
}

}  // namespace common
