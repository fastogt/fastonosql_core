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

#include <fastonosql/core/basic_types.h>

#include <common/string_util.h>

namespace fastonosql {
namespace core {

bool IsKeyMatchPattern(const char* data, size_t size, const pattern_t& pattern) {
  if (!data || size == 0) {
    return false;
  }

  return common::MatchPattern(common::StringPiece(data, size), pattern);
}

const raw_key_t kRangeKeyEnd = raw_key_t();
const raw_key_t kRangeKeyStart = raw_key_t();

bool IsKeyInRange(const raw_key_t& key_start, const raw_key_t& key, const raw_key_t& key_end) {
  if (key.empty()) {  // invalid value
    return false;
  }

  if (key_start == kRangeKeyStart && key_end == kRangeKeyEnd) {  // unlimited range
    return true;
  }

  if (key_start != kRangeKeyStart) {
    if (key_start > key) {
      return false;
    }
  }

  if (key_end != kRangeKeyEnd) {
    if (key_end < key) {
      return false;
    }
  }

  return true;
}

bool IsKeyInEndRange(const raw_key_t& key, const raw_key_t& key_end) {
  return IsKeyInRange(kRangeKeyStart, key, key_end);
}

}  // namespace core
}  // namespace fastonosql
