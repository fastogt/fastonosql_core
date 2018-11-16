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

#include <string>

#include <fastonosql/core/config/config.h>  // for RemoteConfig

namespace fastonosql {
namespace core {
namespace redis_compatible {

struct Config : public RemoteConfig {
  typedef RemoteConfig base_class;

  enum { kDefaultDbNum = 0 };
  explicit Config(const common::net::HostAndPort& host);
  Config();

  void Init(const config_args_t& args);
  config_args_t ToArgs() const;

  std::string hostsocket;
  int db_num;
  std::string auth;
  bool is_ssl;
};

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
