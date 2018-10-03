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

#include <fastonosql/core/db/unqlite/config.h>

extern "C" {
#include "sds/sds_fasto.h"
#include <unqlite.h>
}

#include <common/convert2string.h>
#include <common/file_system/types.h> // for prepare_path
#include <common/sprintf.h>           // for MemSPrintf

#include <fastonosql/core/logger.h>

#define UNQLITE_DEFAULT_ENV_FLAGS (UNQLITE_OPEN_CREATE | UNQLITE_OPEN_READWRITE)

namespace fastonosql {
namespace core {
namespace unqlite {
namespace {

Config ParseOptions(int argc, char **argv) {
  Config cfg;
  for (int i = 0; i < argc; i++) {
    const bool lastarg = i == argc - 1;

    if (!strcmp(argv[i], "-d") && !lastarg) {
      cfg.delimiter = argv[++i];
    } else if (!strcmp(argv[i], "-f") && !lastarg) {
      cfg.db_path = argv[++i];
    } else if (!strcmp(argv[i], "-e") && !lastarg) {
      int env_flags;
      if (common::ConvertFromString(argv[++i], &env_flags)) {
        cfg.env_flags = env_flags;
      }
    } else {
      if (argv[i][0] == '-') {
        const std::string buff =
            common::MemSPrintf("Unrecognized option or bad number of args "
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

} // namespace

Config::Config()
    : LocalConfig(common::file_system::prepare_path("~/test.unqlite")),
      env_flags(UNQLITE_DEFAULT_ENV_FLAGS) {}

bool Config::ReadOnlyDB() const { return env_flags & UNQLITE_OPEN_READONLY; }

void Config::SetReadOnlyDB(bool ro) {
  if (ro) {
    env_flags |= UNQLITE_OPEN_READONLY;
    env_flags &= ~UNQLITE_OPEN_READWRITE;
  } else {
    env_flags &= ~UNQLITE_OPEN_READONLY;
    env_flags |= UNQLITE_OPEN_READWRITE;
  }
}

bool Config::ReadWriteDB() const {
  return env_flags & UNQLITE_OPEN_READWRITE || env_flags & UNQLITE_OPEN_CREATE;
}

void Config::SetReadWriteDB(bool ro) {
  if (ro) {
    env_flags |= UNQLITE_OPEN_READWRITE;
    env_flags &= ~UNQLITE_OPEN_READONLY;
  } else {
    env_flags &= ~UNQLITE_OPEN_READWRITE;
    env_flags |= UNQLITE_OPEN_READONLY;
  }
}

bool Config::CreateIfMissingDB() const {
  return env_flags & UNQLITE_OPEN_CREATE;
}

void Config::SetCreateIfMissingDB(bool ro) {
  if (ro) {
    env_flags |= UNQLITE_OPEN_CREATE;
  } else {
    env_flags &= ~UNQLITE_OPEN_CREATE;
  }
}

} // namespace unqlite
} // namespace core
} // namespace fastonosql

namespace common {

std::string ConvertToString(const fastonosql::core::unqlite::Config &conf) {
  fastonosql::core::config_args_t argv = conf.Args();
  argv.push_back("-e");
  argv.push_back(common::ConvertToString(conf.env_flags));
  return fastonosql::core::ConvertToStringConfigArgs(argv);
}

bool ConvertFromString(const std::string &from,
                       fastonosql::core::unqlite::Config *out) {
  if (!out || from.empty()) {
    return false;
  }

  int argc = 0;
  sds *argv = sdssplitargslong(from.c_str(), &argc);
  if (argv) {
    *out = fastonosql::core::unqlite::ParseOptions(argc, argv);
    sdsfreesplitres(argv, argc);
    return true;
  }

  return false;
}

} // namespace common
