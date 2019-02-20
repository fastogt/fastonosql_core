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

#include <fastonosql/core/db/forestdb/config.h>

#include <common/file_system/types.h>

#define FORESTDB_DB_NAME_FIELD ARGS_FROM_FIELD("n")

namespace fastonosql {
namespace core {
namespace forestdb {

namespace {

const char kDefaultPath[] = "~/test.forestdb";
const char kDefaultDbName[] = "default";

}  // namespace

Config::Config() : LocalConfig(common::file_system::prepare_path(kDefaultPath)), db_name(kDefaultDbName) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == FORESTDB_DB_NAME_FIELD && !lastarg) {
      db_name = args[++i];
      break;
    }
  }
}

config_args_t Config::ToArgs() const {
  fastonosql::core::config_args_t args = base_class::ToArgs();

  if (!db_name.empty()) {
    args.push_back(FORESTDB_DB_NAME_FIELD);
    args.push_back(db_name);
  }

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && db_name == other.db_name;
}

}  // namespace forestdb
}  // namespace core
}  // namespace fastonosql
