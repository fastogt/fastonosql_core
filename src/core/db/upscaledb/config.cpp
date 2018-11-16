/*  Copyright (C) 2014-2017 FastoGT. All right reserved.

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

#include <fastonosql/core/db/upscaledb/config.h>

#include <common/convert2string.h>
#include <common/file_system/types.h>  // for prepare_path

#define UPSCALEDB_DB_NAME_FIELD ARGS_FROM_FIELD("n")
#define UPSCALEDB_CIM_FIELD ARGS_FROM_FIELD("c")

namespace fastonosql {
namespace core {
namespace upscaledb {

namespace {

const char kDefaultPath[] = "~/test.upscaledb";

}  // namespace

Config::Config()
    : LocalConfig(common::file_system::prepare_path(kDefaultPath)), create_if_missing(false), dbnum(kDefaultDbNum) {}

void Config::Init(const config_args_t& args) {
  base_class::Init(args);
  for (size_t i = 0; i < args.size(); i++) {
    const bool lastarg = i == args.size() - 1;
    if (args[i] == UPSCALEDB_DB_NAME_FIELD && !lastarg) {
      uint16_t ldbnum;
      if (common::ConvertFromString(args[++i], &ldbnum)) {
        dbnum = ldbnum;
      }
    } else if (args[i] == UPSCALEDB_CIM_FIELD) {
      create_if_missing = true;
    }
  }
}

config_args_t Config::ToArgs() const {
  config_args_t args = base_class::ToArgs();

  if (create_if_missing) {
    args.push_back(UPSCALEDB_CIM_FIELD);
  }

  args.push_back(UPSCALEDB_DB_NAME_FIELD);
  args.push_back(common::ConvertToString(dbnum));

  return args;
}

bool Config::Equals(const Config& other) const {
  return base_class::Equals(other) && create_if_missing == other.create_if_missing && dbnum == other.dbnum;
}

}  // namespace upscaledb
}  // namespace core
}  // namespace fastonosql
