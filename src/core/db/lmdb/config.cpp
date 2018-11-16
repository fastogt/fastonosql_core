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

#include <fastonosql/core/db/lmdb/config.h>

#include <lmdb.h>  // for mdb_txn_abort, MDB_val

#include <common/convert2string.h>
#include <common/file_system/types.h>  // for prepare_path

#define LMDB_DEFAULT_ENV_FLAGS MDB_NOSUBDIR

#define LMDB_DB_NAME_FIELD ARGS_FROM_FIELD("n")
#define LMDB_MAX_DBS_FIELD ARGS_FROM_FIELD("m")
#define LMDB_ENV_FIELD ARGS_FROM_FIELD("e")

namespace fastonosql {
namespace core {
namespace lmdb {

namespace {

const char kDefaultPath[] = "~/test.lmdb";
const char kDefaultDbName[] = "default";

}  // namespace

Config::Config()
    : LocalConfig(common::file_system::prepare_path(kDefaultPath)),
      env_flags(LMDB_DEFAULT_ENV_FLAGS),
      db_name(kDefaultDbName),
      max_dbs(default_dbs_count) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == LMDB_DB_NAME_FIELD && !lastarg) {
      db_name = args[++i];
    } else if (args[i] == LMDB_ENV_FIELD && !lastarg) {
      unsigned int lenv;
      if (common::ConvertFromString(args[++i], &lenv)) {
        env_flags = lenv;
      }
    } else if (args[i] == LMDB_MAX_DBS_FIELD && !lastarg) {
      unsigned int lmax_dbs;
      if (common::ConvertFromString(args[++i], &lmax_dbs)) {
        max_dbs = lmax_dbs;
      }
    }
  }
}

config_args_t Config::ToArgs() const {
  fastonosql::core::config_args_t args = base_class::ToArgs();

  args.push_back(LMDB_ENV_FIELD);
  args.push_back(common::ConvertToString(env_flags));

  if (!db_name.empty()) {
    args.push_back(LMDB_DB_NAME_FIELD);
    args.push_back(db_name);
  }

  args.push_back(LMDB_MAX_DBS_FIELD);
  args.push_back(common::ConvertToString(max_dbs));

  return args;
}

bool Config::ReadOnlyDB() const {
  return env_flags & MDB_RDONLY;
}

bool Config::IsSingleFileDB() const {
  return env_flags & MDB_NOSUBDIR;
}

void Config::SetSingleFileDB(bool single) {
  if (single) {
    env_flags |= MDB_NOSUBDIR;
  } else {
    env_flags &= ~MDB_NOSUBDIR;
  }
}

void Config::SetReadOnlyDB(bool ro) {
  if (ro) {
    env_flags |= MDB_RDONLY;
  } else {
    env_flags &= ~MDB_RDONLY;
  }
}

}  // namespace lmdb
}  // namespace core
}  // namespace fastonosql
