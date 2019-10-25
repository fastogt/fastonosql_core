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
#include <vector>

#define ARGS_FROM_FIELD(x) "-" x

#define DELIMITER_FIELD ARGS_FROM_FIELD("d")
#define DB_PATH_FIELD ARGS_FROM_FIELD("f")
#define HOST_FIELD ARGS_FROM_FIELD("h")
#define PORT_FIELD ARGS_FROM_FIELD("p")

namespace fastonosql {
namespace core {

typedef std::vector<std::string> config_args_t;

extern const char kArgsSeparator[];

// -d
struct BaseConfig {
  static const char default_delimiter[];

  BaseConfig();

  void Init(const config_args_t& args);
  config_args_t ToArgs() const;

  bool Equals(const BaseConfig& other) const;

  std::string delimiter;
};

inline bool operator==(const BaseConfig& r, const BaseConfig& l) {
  return r.Equals(l);
}

inline bool operator!=(const BaseConfig& r, const BaseConfig& l) {
  return !(r == l);
}

bool ConvertToConfigArgsString(const std::string& line, config_args_t* args);
bool ConvertToStringConfigArgs(const config_args_t& args, std::string* out);

}  // namespace core
}  // namespace fastonosql
