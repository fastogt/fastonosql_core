/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

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

#include <common/net/types.h>

#include <fastonosql/core/connection_types.h>

namespace fastonosql {
namespace core {

struct ServerCommonInfo {
  ServerCommonInfo();
  ServerCommonInfo(const std::string& name, ServerType type, ServerState state, ServerConnectionState cstate);

  std::string name;              // name
  ServerType type;               // role
  ServerState state;             // state
  ServerConnectionState cstate;  // conection state
  common::net::HostAndPortAndSlot host;
};

class ServerDiscoveryInfoBase {
 public:
  ConnectionType GetConnectionType() const;
  ServerCommonInfo GetInfo() const;

  std::string GetName() const;
  void SetName(const std::string& name);

  common::net::HostAndPortAndSlot GetHost() const;
  void SetHost(const common::net::HostAndPortAndSlot& host);

 protected:
  ServerDiscoveryInfoBase(ConnectionType ctype, const ServerCommonInfo& info);
  ~ServerDiscoveryInfoBase();

 private:
  const ConnectionType ctype_;
  ServerCommonInfo info_;
};

}  // namespace core
}  // namespace fastonosql
