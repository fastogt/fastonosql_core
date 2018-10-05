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

#include "core/db/upscaledb/internal/commands_api.h"

#include <fastonosql/core/db/upscaledb/db_connection.h>

namespace fastonosql {
namespace core {
namespace upscaledb {

common::Error CommandsApi::Info(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* mdb = static_cast<DBConnection*>(handler);
  ServerInfo::Stats statsout;
  common::Error err = mdb->Info(argv.size() == 1 ? argv[0] : std::string(), &statsout);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(ServerInfo(statsout).ToString());
  FastoObject* child = new FastoObject(out, val, mdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace upscaledb
}  // namespace core
}  // namespace fastonosql
