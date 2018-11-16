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

#pragma once

#include <string>
#include <vector>

#include <fastonosql/core/config/local_config.h>

namespace fastonosql {
namespace core {
namespace rocksdb {

enum ComparatorType : uint8_t { COMP_BYTEWISE, COMP_REVERSE_BYTEWISE };
extern const std::vector<const char*> g_comparator_types;

enum CompressionType : uint8_t {
  kNoCompression = 0x0,
  kSnappyCompression = 0x1,
  kZlibCompression = 0x2,
  kBZip2Compression = 0x3,
  kLZ4Compression = 0x4,
  kLZ4HCCompression = 0x5,
  kXpressCompression = 0x6,
  kZSTD = 0x7
};
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

}  // namespace rocksdb
}  // namespace core
}  // namespace fastonosql

namespace common {
std::string ConvertToString(fastonosql::core::rocksdb::ComparatorType comp);
bool ConvertFromString(const std::string& from, fastonosql::core::rocksdb::ComparatorType* out);

std::string ConvertToString(fastonosql::core::rocksdb::CompressionType comp);
bool ConvertFromString(const std::string& from, fastonosql::core::rocksdb::CompressionType* out);
}  // namespace common
