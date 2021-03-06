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

#pragma once

#include <string>
#include <vector>

#include <fastonosql/core/config/local_config.h>

namespace fastonosql {
namespace core {
namespace leveldb {

enum ComparatorType : uint8_t { COMP_BYTEWISE = 0, COMP_INDEXED_DB };
extern const std::vector<const char*> g_comparator_types;

enum CompressionType : uint8_t { kNoCompression = 0, kSnappyCompression };
extern const std::vector<const char*> g_compression_types;

struct Config : public LocalConfig {
  typedef LocalConfig base_class;

  Config();

  void Init(const config_args_t& args);
  config_args_t ToArgs() const;

  bool Equals(const Config& other) const;

  bool create_if_missing;
  ComparatorType comparator;
  CompressionType compression;
};

inline bool operator==(const Config& r, const Config& l) {
  return r.Equals(l);
}

inline bool operator!=(const Config& r, const Config& l) {
  return !(r == l);
}

}  // namespace leveldb
}  // namespace core
}  // namespace fastonosql

namespace common {
std::string ConvertToString(fastonosql::core::leveldb::ComparatorType comp);
bool ConvertFromString(const std::string& from, fastonosql::core::leveldb::ComparatorType* out);

std::string ConvertToString(fastonosql::core::leveldb::CompressionType comp);
bool ConvertFromString(const std::string& from, fastonosql::core::leveldb::CompressionType* out);
}  // namespace common
