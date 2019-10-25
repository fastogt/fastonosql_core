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

#include <fastonosql/core/config/local_config.h>

namespace fastonosql {
namespace core {

LocalConfig::LocalConfig() : LocalConfig(std::string()) {}

LocalConfig::LocalConfig(const std::string& db_path) : base_class(), db_path(db_path) {}

void LocalConfig::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == DB_PATH_FIELD && !lastarg) {
      db_path = args[++i];
      break;
    }
  }
}

config_args_t LocalConfig::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (!db_path.empty()) {
    args.push_back(DB_PATH_FIELD);
    args.push_back(db_path);
  }

  return args;
}

bool LocalConfig::Equals(const LocalConfig& other) const {
  return base_class::Equals(other) && db_path == other.db_path;
}

}  // namespace core
}  // namespace fastonosql
