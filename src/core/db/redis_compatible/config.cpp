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

#include <fastonosql/core/db/redis_compatible/config.h>

#include <common/convert2string.h>

#define REDIS_DB_NAME_FIELD ARGS_FROM_FIELD("n")
#define REDIS_HOST_SOCKET_FIELD ARGS_FROM_FIELD("s")
#define REDIS_AUTH_FIELD ARGS_FROM_FIELD("a")
#define REDIS_SSL_FIELD ARGS_FROM_FIELD("ssl")

namespace fastonosql {
namespace core {
namespace redis_compatible {

Config::Config(const common::net::HostAndPort& host)
    : RemoteConfig(host), hostsocket(), db_num(kDefaultDbNum), auth(), is_ssl(false) {}

Config::Config() : Config(common::net::HostAndPort()) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == REDIS_DB_NAME_FIELD && !lastarg) {
      int ldbnum;
      if (common::ConvertFromString(args[++i], &ldbnum)) {
        db_num = ldbnum;
      }
    } else if (args[i] == REDIS_HOST_SOCKET_FIELD && !lastarg) {
      hostsocket = args[++i];
    } else if (args[i] == REDIS_AUTH_FIELD && !lastarg) {
      auth = args[++i];
    } else if (args[i] == REDIS_SSL_FIELD) {
      is_ssl = true;
    }
  }
}

config_args_t Config::ToArgs() const {
  fastonosql::core::config_args_t args = base_class::ToArgs();

  if (!hostsocket.empty()) {
    args.push_back(REDIS_HOST_SOCKET_FIELD);
    args.push_back(hostsocket);
  }

  args.push_back(REDIS_DB_NAME_FIELD);
  args.push_back(common::ConvertToString(db_num));

  if (!auth.empty()) {
    args.push_back(REDIS_AUTH_FIELD);
    args.push_back(auth);
  }

  if (is_ssl) {
    args.push_back(REDIS_SSL_FIELD);
  }

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && hostsocket == other.hostsocket && db_num == other.db_num && auth == other.auth &&
         is_ssl == other.is_ssl;
}

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
