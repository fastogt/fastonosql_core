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

#include <fastonosql/core/db/ssdb/config.h>

#define DEFAULT_SSDB_SERVER_PORT 8888

#define SSDB_AUTH_FIELD ARGS_FROM_FIELD("a")

namespace fastonosql {
namespace core {
namespace ssdb {
namespace {

const common::net::HostAndPort kDefaultHost = common::net::HostAndPort::CreateLocalHost(DEFAULT_SSDB_SERVER_PORT);

}  // namespace

Config::Config() : RemoteConfig(kDefaultHost), auth() {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == SSDB_AUTH_FIELD && !lastarg) {
      auth = args[++i];
      break;
    }
  }
}

config_args_t Config::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (!auth.empty()) {
    args.push_back(SSDB_AUTH_FIELD);
    args.push_back(auth);
  }

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && auth == other.auth;
}

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
