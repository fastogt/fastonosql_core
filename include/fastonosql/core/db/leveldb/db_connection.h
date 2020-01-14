/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

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

#include <fastonosql/core/cdb_connection.h>

#include <fastonosql/core/db/leveldb/config.h>
#include <fastonosql/core/db/leveldb/server_info.h>

namespace leveldb {
class DB;
class Status;
}  // namespace leveldb

namespace fastonosql {
namespace core {
namespace leveldb {

typedef ::leveldb::DB NativeConnection;

common::Error CreateConnection(const Config& config, NativeConnection** context);
common::Error TestConnection(const Config& config);

class DBConnection : public CDBConnection<NativeConnection, Config, LEVELDB> {
 public:
  typedef CDBConnection<NativeConnection, Config, LEVELDB> base_class;
  explicit DBConnection(CDBConnectionClient* client);

  common::Error Info(ServerInfo::Stats* statsout) WARN_UNUSED_RESULT;
  common::Error Info(std::string* statsout) WARN_UNUSED_RESULT;
  common::Error GetProperty(const std::string& property, std::string* out) WARN_UNUSED_RESULT;

  IServerInfo* MakeServerInfo(const std::string& content) const override;
  IDataBaseInfo* MakeDatabaseInfo(const db_name_t& name, bool is_default, size_t size) const override;

 private:
  common::Error CheckResultCommand(const std::string& cmd, const ::leveldb::Status& err) WARN_UNUSED_RESULT;

  common::Error DelInner(const raw_key_t& key) WARN_UNUSED_RESULT;
  common::Error SetInner(const raw_key_t& key, const raw_value_t& value) WARN_UNUSED_RESULT;
  common::Error GetInner(const raw_key_t& key, raw_value_t* ret_val) WARN_UNUSED_RESULT;

  common::Error ScanImpl(cursor_t cursor_in,
                         const pattern_t& pattern,
                         keys_limit_t count_keys,
                         raw_keys_t* keys_out,
                         cursor_t* cursor_out) override;
  common::Error KeysImpl(const raw_key_t& key_start,
                         const raw_key_t& key_end,
                         cursor_t limit,
                         raw_keys_t* ret) override;
  common::Error DBKeysCountImpl(keys_limit_t* size) override;
  common::Error FlushDBImpl() override;
  common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) override;
  common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) override;
  common::Error SetImpl(const NDbKValue& key) override;
  common::Error GetImpl(const NKey& key, NDbKValue* loaded_key) override;
  common::Error RenameImpl(const NKey& key, const nkey_t& new_key) override;
  common::Error QuitImpl() override;
  common::Error ConfigGetDatabasesImpl(db_names_t* dbs) override;
};

}  // namespace leveldb
}  // namespace core
}  // namespace fastonosql
