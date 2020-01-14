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

#include <fastonosql/core/config/remote_config.h>

#include <common/convert2string.h>

namespace fastonosql {
namespace core {

RemoteConfig::RemoteConfig() : RemoteConfig(common::net::HostAndPort()) {}

RemoteConfig::RemoteConfig(const common::net::HostAndPort& host) : base_class(), host(host) {}

void RemoteConfig::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == HOST_FIELD && !lastarg) {
      host.SetHost(args[++i]);
    } else if (args[i] == PORT_FIELD && !lastarg) {
      common::net::HostAndPort::port_t port;
      if (common::ConvertFromString(args[++i], &port)) {
        host.SetPort(port);
      }
    }
  }
}

config_args_t RemoteConfig::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (host.IsValid()) {
    args.push_back(HOST_FIELD);
    args.push_back(host.GetHost());
    args.push_back(PORT_FIELD);
    args.push_back(common::ConvertToString(host.GetPort()));
  }

  return args;
}

bool RemoteConfig::Equals(const RemoteConfig& other) const {
  return base_class::Equals(other) && host == other.host;
}

}  // namespace core
}  // namespace fastonosql
