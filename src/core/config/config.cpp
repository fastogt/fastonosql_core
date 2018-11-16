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

#include <fastonosql/core/config/config.h>

#include <common/convert2string.h>
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

LocalConfig::LocalConfig(const std::string& db_path) : BaseConfig(), db_path(db_path) {}

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

RemoteConfig::RemoteConfig(const common::net::HostAndPort& host) : BaseConfig(), host(host) {}

void RemoteConfig::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == HOST_FIELD && !lastarg) {
      host.SetHost(args[++i]);
    } else if (args[i] == PORT_FIELD && !lastarg) {
      common::net::HostAndPort::port_t port;
      if (common::ConvertFromString(args[++i], &port)) {
        host.SetPort(port);
      }
    }
  }
}

config_args_t RemoteConfig::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (host.IsValid()) {
    args.push_back(HOST_FIELD);
    args.push_back(host.GetHost());
    args.push_back(PORT_FIELD);
    args.push_back(common::ConvertToString(host.GetPort()));
  }

  return args;
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
