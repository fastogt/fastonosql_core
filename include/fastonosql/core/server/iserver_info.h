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
#include <string>

#include <common/net/types.h>  // for HostAndPortAndSlot
#include <common/types.h>      // for time64_t

namespace common {
class Value;
}

namespace fastonosql {
namespace core {

class IServerInfo {
 public:
  IServerInfo();
  virtual ~IServerInfo();

  virtual std::string ToString() const = 0;
  virtual uint32_t GetVersion() const = 0;
  virtual common::Value* GetValueByIndexes(unsigned char property, unsigned char field) const = 0;
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
