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

#include <fastonosql/core/server/server_discovery_info.h>

namespace fastonosql {
namespace core {

ServerCommonInfo::ServerCommonInfo() : name(), type(MASTER), state(SUP), cstate(SCONNECTED), host() {}

ServerCommonInfo::ServerCommonInfo(const std::string& name,
                                   ServerType type,
                                   ServerState state,
                                   ServerConnectionState cstate)
    : name(name), type(type), state(state), cstate(cstate) {}

ServerDiscoveryInfoBase::ServerDiscoveryInfoBase(ConnectionType ctype, const ServerCommonInfo& info)
    : ctype_(ctype), info_(info) {}

ConnectionType ServerDiscoveryInfoBase::GetConnectionType() const {
  return ctype_;
}

ServerCommonInfo ServerDiscoveryInfoBase::GetInfo() const {
  return info_;
}

std::string ServerDiscoveryInfoBase::GetName() const {
  return info_.name;
}

void ServerDiscoveryInfoBase::SetName(const std::string& name) {
  info_.name = name;
}

common::net::HostAndPortAndSlot ServerDiscoveryInfoBase::GetHost() const {
  return info_.host;
}

void ServerDiscoveryInfoBase::SetHost(const common::net::HostAndPortAndSlot& host) {
  info_.host = host;
}

ServerDiscoveryInfoBase::~ServerDiscoveryInfoBase() {}

}  // namespace core
}  // namespace fastonosql
