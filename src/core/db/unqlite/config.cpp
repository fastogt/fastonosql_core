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
#include <unqlite.h>
}

#include <common/convert2string.h>
#include <common/file_system/types.h>  // for prepare_path

#define UNQLITE_DEFAULT_ENV_FLAGS (UNQLITE_OPEN_CREATE | UNQLITE_OPEN_READWRITE)

#define UNQLITE_ENV_FIELD ARGS_FROM_FIELD("e")

namespace fastonosql {
namespace core {
namespace unqlite {
namespace {

const char kDefaultPath[] = "~/test.unqlite";

}  // namespace

Config::Config() : LocalConfig(common::file_system::prepare_path(kDefaultPath)), env_flags(UNQLITE_DEFAULT_ENV_FLAGS) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == UNQLITE_ENV_FIELD && !lastarg) {
      unsigned int lenv;
      if (common::ConvertFromString(args[++i], &lenv)) {
        env_flags = lenv;
      }
      break;
    }
  }
}

config_args_t Config::ToArgs() const {
  fastonosql::core::config_args_t args = base_class::ToArgs();

  args.push_back(UNQLITE_ENV_FIELD);
  args.push_back(common::ConvertToString(env_flags));

  return args;
}

bool Config::ReadOnlyDB() const {
  return env_flags & UNQLITE_OPEN_READONLY;
}

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

}  // namespace unqlite
}  // namespace core
}  // namespace fastonosql
