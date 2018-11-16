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

#include <fastonosql/core/config/config.h>

#define DB_PATH_FIELD ARGS_FROM_FIELD("f")

namespace fastonosql {
namespace core {

// -f -d
struct LocalConfig : public BaseConfig {
  typedef BaseConfig base_class;

  LocalConfig();
  explicit LocalConfig(const std::string& db_path);

  void Init(const config_args_t& args);
  config_args_t ToArgs() const;

  bool Equals(const LocalConfig& other) const;

  std::string db_path;
};

inline bool operator==(const LocalConfig& r, const LocalConfig& l) {
  return r.Equals(l);
}

inline bool operator!=(const LocalConfig& r, const LocalConfig& l) {
  return !(r == l);
}

}  // namespace core
}  // namespace fastonosql
