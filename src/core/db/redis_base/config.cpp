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

#include <fastonosql/core/db/redis_base/config.h>

namespace fastonosql {
namespace core {
namespace redis_base {

Config::Config(const common::net::HostAndPort& host) : base_class(host) {}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other);
}

}  // namespace redis_base
}  // namespace core
}  // namespace fastonosql
