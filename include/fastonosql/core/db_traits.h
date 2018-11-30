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
#include <utility>
#include <vector>

#include <common/value.h>  // for Value, Value::Type

#include <fastonosql/core/connection_types.h>  // for ConnectionType

namespace fastonosql {
namespace core {

struct Field {
  Field(const std::string& name, common::Value::Type type);

  bool IsIntegral() const;
  std::string name;
  common::Value::Type type;
};

typedef std::pair<std::string, std::vector<Field>> info_field_t;

std::vector<common::Value::Type> GetSupportedValueTypes(ConnectionType type, uint32_t server_version);
std::vector<info_field_t> GetInfoFieldsFromType(ConnectionType type);

}  // namespace core
}  // namespace fastonosql
