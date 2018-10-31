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

#include <fastonosql/core/config/config.h>  // for RemoteConfig

namespace fastonosql {
namespace core {
namespace redis_compatible {

struct Config : public RemoteConfig {
  enum { kDefaultDbNum = 0 };
  explicit Config(const common::net::HostAndPort& host);
  Config();

  std::string hostsocket;
  int db_num;
  std::string auth;
  bool is_ssl;
};

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql

namespace common {
std::string ConvertToString(const fastonosql::core::redis_compatible::Config& conf);
bool ConvertFromString(const std::string& from, fastonosql::core::redis_compatible::Config* out);
}  // namespace common
