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

#include <fastonosql/core/config/config.h>

namespace fastonosql {
namespace core {
namespace leveldb {

enum ComparatorType { COMP_BYTEWISE = 0, COMP_INDEXED_DB };
extern const std::vector<const char *> g_comparator_types;

enum CompressionType { kNoCompression, kSnappyCompression };
extern const std::vector<const char *> g_compression_types;

struct Config : public LocalConfig {
  Config();

  bool create_if_missing;
  ComparatorType comparator;
  CompressionType compression;
};

} // namespace leveldb
} // namespace core
} // namespace fastonosql

namespace common {
std::string ConvertToString(const fastonosql::core::leveldb::Config &conf);
bool ConvertFromString(const std::string &from,
                       fastonosql::core::leveldb::Config *out);

std::string ConvertToString(fastonosql::core::leveldb::ComparatorType comp);
bool ConvertFromString(const std::string &from,
                       fastonosql::core::leveldb::ComparatorType *out);

std::string ConvertToString(fastonosql::core::leveldb::CompressionType comp);
bool ConvertFromString(const std::string &from,
                       fastonosql::core::leveldb::CompressionType *out);
} // namespace common
