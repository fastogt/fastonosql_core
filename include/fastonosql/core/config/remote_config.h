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

#include <common/net/types.h>

#include <fastonosql/core/config/config.h>

#define HOST_FIELD ARGS_FROM_FIELD("h")
#define PORT_FIELD ARGS_FROM_FIELD("p")

namespace fastonosql {
namespace core {

// -h -p -d
struct RemoteConfig : public BaseConfig {
  typedef BaseConfig base_class;

  RemoteConfig();
  explicit RemoteConfig(const common::net::HostAndPort& host);

  void Init(const config_args_t& args);
  config_args_t ToArgs() const;

  bool Equals(const RemoteConfig& other) const;

  common::net::HostAndPort host;
};

inline bool operator==(const RemoteConfig& r, const RemoteConfig& l) {
  return r.Equals(l);
}

inline bool operator!=(const RemoteConfig& r, const RemoteConfig& l) {
  return !(r == l);
}

}  // namespace core
}  // namespace fastonosql
