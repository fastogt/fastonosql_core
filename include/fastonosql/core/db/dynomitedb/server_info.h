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
#include <vector>

#include <fastonosql/core/db/redis/server_info.h>

namespace fastonosql {
namespace core {
namespace dynomitedb {

std::vector<common::Value::Type> GetSupportedValueTypes();
std::vector<info_field_t> GetInfoFields();

class ServerInfo : public redis::ServerInfo {
 public:
  typedef redis::ServerInfo base_class;

  ServerInfo() : base_class() {}
  ServerInfo(const Server& serv,
             const Clients& clients,
             const Memory& memory,
             const Persistence& pers,
             const Stats& stats,
             const Replication& repl,
             const Cpu& cpu,
             const Keyspace& key)
      : base_class(serv, clients, memory, pers, stats, repl, cpu, key) {}
  explicit ServerInfo(const std::string& content) : base_class(content) {}
};

}  // namespace dynomitedb
}  // namespace core
}  // namespace fastonosql
