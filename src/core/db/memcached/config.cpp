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

#include <fastonosql/core/db/memcached/config.h>

#define DEFAULT_MEMCACHED_SERVER_PORT 11211

#define MEMCACHED_USER_FIELD ARGS_FROM_FIELD("u")
#define MEMCACHED_PASSWORD_FIELD ARGS_FROM_FIELD("a")

namespace fastonosql {
namespace core {
namespace memcached {

namespace {

const common::net::HostAndPort kDefaultHost =
    common::net::HostAndPort::CreateLocalHostIPV4(DEFAULT_MEMCACHED_SERVER_PORT);

}  // namespace

Config::Config() : RemoteConfig(kDefaultHost), user(), password() {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == MEMCACHED_USER_FIELD && !lastarg) {
      user = args[++i];
    } else if (args[i] == MEMCACHED_PASSWORD_FIELD && !lastarg) {
      password = args[++i];
    }
  }
}

config_args_t Config::ToArgs() const {
  fastonosql::core::config_args_t args = base_class::ToArgs();

  if (!user.empty()) {
    args.push_back(MEMCACHED_USER_FIELD);
    args.push_back(user);
  }

  if (!password.empty()) {
    args.push_back(MEMCACHED_PASSWORD_FIELD);
    args.push_back(password);
  }

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && user == other.user && password == other.password;
}

}  // namespace memcached
}  // namespace core
}  // namespace fastonosql
