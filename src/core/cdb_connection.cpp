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

#include <fastonosql/core/cdb_connection.h>

#include <common/convert2string.h>

namespace fastonosql {
namespace core {
namespace detail {

bool StableForJson(const ReadableString& data, readable_string_t* out) {
  readable_string_t data_raw = data.GetData();
  if (data_raw.empty() || !out) {
    DNOTREACHED();
    return false;
  }

  ReadableString::DataType type = data.GetType();
  if (type == ReadableString::BINARY_DATA) {
    std::string hexed;
    bool is_ok = common::utils::xhex::encode(data_raw, ReadableString::is_lower_hex, &hexed);
    DCHECK(is_ok) << "Can't hexed: " << data_raw;
    *out = common::MemSPrintf("'%s'", hexed);
    return true;
  }

  if (detail::is_json(data_raw)) {
    *out = data_raw;
    return true;
  }

  *out = common::MemSPrintf("'%s'", data_raw);
  return true;
}

}  // namespace detail
}  // namespace core
}  // namespace fastonosql
