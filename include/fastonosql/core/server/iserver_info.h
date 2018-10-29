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

#include <memory>  // for shared_ptr

#include <common/net/types.h>  // for HostAndPortAndSlot
#include <common/types.h>      // for time64_t

#include <fastonosql/core/connection_types.h>  // for ConnectionType, etc

namespace common {
class Value;
}

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

class ServerDiscoverySentinelInfo : public ServerDiscoveryInfoBase {
 public:
  virtual ~ServerDiscoverySentinelInfo();

 protected:
  ServerDiscoverySentinelInfo(ConnectionType ctype, const ServerCommonInfo& info);
};

typedef std::shared_ptr<ServerDiscoverySentinelInfo> ServerDiscoverySentinelInfoSPtr;

class ServerDiscoveryClusterInfo : public ServerDiscoveryInfoBase {
 public:
  bool Self() const;
  virtual ~ServerDiscoveryClusterInfo();

 protected:
  ServerDiscoveryClusterInfo(ConnectionType ctype, const ServerCommonInfo& info, bool self);

 private:
  const bool self_;
};

typedef std::shared_ptr<ServerDiscoveryClusterInfo> ServerDiscoveryClusterInfoSPtr;

struct IStateField {
  virtual common::Value* GetValueByIndex(unsigned char index) const = 0;
  virtual ~IStateField();
};

class IServerInfo {
 public:
  explicit IServerInfo(ConnectionType type);
  virtual ~IServerInfo();

  ConnectionType GetType() const;
  virtual std::string ToString() const = 0;
  virtual uint32_t GetVersion() const = 0;
  virtual common::Value* GetValueByIndexes(unsigned char property, unsigned char field) const = 0;

 private:
  const ConnectionType type_;
};

typedef std::shared_ptr<IServerInfo> IServerInfoSPtr;

struct ServerInfoSnapShoot {
  ServerInfoSnapShoot();
  ServerInfoSnapShoot(common::time64_t msec, IServerInfoSPtr info);
  bool IsValid() const;

  common::time64_t msec;
  IServerInfoSPtr info;
};

}  // namespace core
}  // namespace fastonosql
