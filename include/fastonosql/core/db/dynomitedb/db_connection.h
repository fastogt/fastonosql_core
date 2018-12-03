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

#include <vector>

#include <fastonosql/core/db/redis_compatible/db_connection.h>

#include <fastonosql/core/db/dynomitedb/config.h>

namespace fastonosql {
namespace core {
namespace dynomitedb {

typedef redis_compatible::NativeConnection NativeConnection;

common::Error CreateConnection(const RConfig& config, NativeConnection** context);
common::Error TestConnection(const RConfig& config);

class DBConnection : public redis_compatible::DBConnection<RConfig, DYNOMITEDB> {
 public:
  typedef redis_compatible::DBConnection<RConfig, DYNOMITEDB> base_class;
  explicit DBConnection(CDBConnectionClient* client);

 private:
  common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) override;
  common::Error DBKeysCountImpl(keys_limit_t* size) override;
};

}  // namespace dynomitedb
}  // namespace core
}  // namespace fastonosql
