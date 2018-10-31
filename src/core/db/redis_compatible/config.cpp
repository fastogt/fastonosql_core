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

#include <fastonosql/core/db/redis_compatible/config.h>

extern "C" {
#include "sds/sds_fasto.h"
}

#include <common/convert2string.h>  // for ConvertToString, etc
#include <common/sprintf.h>         // for MemSPrintf

#include <fastonosql/core/logger.h>

namespace fastonosql {
namespace core {
namespace redis_compatible {
namespace {

Config ParseOptions(int argc, char** argv) {
  Config cfg;
  for (int i = 0; i < argc; i++) {
    const bool lastarg = i == argc - 1;

    if (!strcmp(argv[i], "-h") && !lastarg) {
      cfg.host.SetHost(argv[++i]);
    } else if (!strcmp(argv[i], "-p") && !lastarg) {
      uint16_t lport;
      if (common::ConvertFromString(std::string(argv[++i]), &lport)) {
        cfg.host.SetPort(lport);
      }
    } else if (!strcmp(argv[i], "-s") && !lastarg) {
      cfg.hostsocket = argv[++i];
    } else if (!strcmp(argv[i], "-n") && !lastarg) {
      int ldbnum;
      if (common::ConvertFromString(std::string(argv[++i]), &ldbnum)) {
        cfg.db_num = ldbnum;
      }
    } else if (!strcmp(argv[i], "-a") && !lastarg) {
      cfg.auth = argv[++i];
    } else if (!strcmp(argv[i], "-d") && !lastarg) {
      cfg.delimiter = argv[++i];
    } else if (!strcmp(argv[i], "-ssl")) {
      cfg.is_ssl = true;
    } else {
      if (argv[i][0] == '-') {
        const std::string buff = common::MemSPrintf(
            "Unrecognized option or bad number of args "
            "for: '%s'",
            argv[i]);
        LOG_CORE_MSG(buff, common::logging::LOG_LEVEL_WARNING, true);
        break;
      } else {
        /* Likely the command name, stop here. */
        break;
      }
    }
  }

  return cfg;
}

}  // namespace

Config::Config(const common::net::HostAndPort& host)
    : RemoteConfig(host), hostsocket(), db_num(kDefaultDbNum), auth(), is_ssl(false) {}

Config::Config() : Config(common::net::HostAndPort()) {}

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql

namespace common {

std::string ConvertToString(const fastonosql::core::redis_compatible::Config& conf) {
  fastonosql::core::config_args_t argv = conf.Args();

  if (!conf.hostsocket.empty()) {
    argv.push_back("-s");
    argv.push_back(conf.hostsocket);
  }
  argv.push_back("-n");
  argv.push_back(ConvertToString(conf.db_num));

  if (!conf.auth.empty()) {
    argv.push_back("-a");
    argv.push_back(conf.auth);
  }

  if (conf.is_ssl) {
    argv.push_back("-ssl");
  }

  return fastonosql::core::ConvertToStringConfigArgs(argv);
}

bool ConvertFromString(const std::string& from, fastonosql::core::redis_compatible::Config* out) {
  if (!out || from.empty()) {
    return false;
  }

  int argc = 0;
  sds* argv = sdssplitargslong(from.c_str(), &argc);
  if (argv) {
    *out = fastonosql::core::redis_compatible::ParseOptions(argc, argv);
    sdsfreesplitres(argv, argc);
    return true;
  }

  return false;
}

}  // namespace common
