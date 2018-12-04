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

#include <fastonosql/core/db/dynomitedb/server_info.h>

namespace fastonosql {
namespace core {
namespace dynomitedb {

std::vector<common::Value::Type> GetSupportedValueTypes() {
  return {common::Value::TYPE_STRING, common::Value::TYPE_ARRAY, common::Value::TYPE_SET, common::Value::TYPE_ZSET,
          common::Value::TYPE_HASH};
}

std::vector<info_field_t> GetInfoFields() {
  return redis::GetInfoFields();
}

}  // namespace dynomitedb
}  // namespace core
}  // namespace fastonosql
