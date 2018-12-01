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

#include <fastonosql/core/cluster/cluster_discovery_info.h>

namespace fastonosql {
namespace core {

ServerDiscoveryClusterInfo::ServerDiscoveryClusterInfo(ConnectionType ctype, const ServerCommonInfo& info, bool self)
    : ServerDiscoveryInfoBase(ctype, info), self_(self) {}

ServerDiscoveryClusterInfo::~ServerDiscoveryClusterInfo() {}

bool ServerDiscoveryClusterInfo::Self() const {
  return self_;
}

}  // namespace core
}  // namespace fastonosql
