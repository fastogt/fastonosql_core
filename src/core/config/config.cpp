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

#include <fastonosql/core/config/config.h>

#include <common/string_util.h>

namespace fastonosql {
namespace core {

extern const char kArgsSeparator[] = " ";

const char BaseConfig::default_delimiter[] = "\n";

BaseConfig::BaseConfig() : delimiter(default_delimiter) {}

void BaseConfig::Init(const config_args_t& args) {
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == DELIMITER_FIELD && !lastarg) {
      delimiter = args[++i];
      break;
    }
  }
}

config_args_t BaseConfig::ToArgs() const {
  config_args_t args;

  if (!delimiter.empty()) {
    args.push_back(DELIMITER_FIELD);
    args.push_back(delimiter);
  }

  return args;
}

bool BaseConfig::Equals(const BaseConfig& other) const {
  return delimiter == other.delimiter;
}

bool ConvertToConfigArgsString(const std::string& line, config_args_t* args) {
  if (!args) {
    return false;
  }

  config_args_t largs;
  common::Tokenize(line, kArgsSeparator, &largs);
  *args = largs;
  return true;
}

bool ConvertToStringConfigArgs(const config_args_t& args, std::string* out) {
  if (!out) {
    return false;
  }

  *out = common::JoinString(args, kArgsSeparator);
  return true;
}

}  // namespace core
}  // namespace fastonosql
