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

#include <libforestdb/fdb_errors.h>

#include <fastonosql/core/cdb_connection.h>  // for CDBConnection

#include <fastonosql/core/db/forestdb/config.h>
#include <fastonosql/core/db/forestdb/server_info.h>  // for ServerInfo

namespace fastonosql {
namespace core {
namespace forestdb {

struct fdb;
typedef fdb NativeConnection;

common::Error CreateConnection(const Config& config, NativeConnection** context);
common::Error TestConnection(const Config& config);

// path is file
class DBConnection : public CDBConnection<NativeConnection, Config, FORESTDB> {
 public:
  typedef CDBConnection<NativeConnection, Config, FORESTDB> base_class;
  explicit DBConnection(CDBConnectionClient* client);

  virtual db_name_t GetCurrentDBName() const override;
  common::Error Info(const command_buffer_t& args, ServerInfo::Stats* statsout) WARN_UNUSED_RESULT;

 private:
  common::Error CheckResultCommand(const std::string& cmd, fdb_status err) WARN_UNUSED_RESULT;

  common::Error SetInner(const raw_key_t& key, const raw_value_t& value) WARN_UNUSED_RESULT;
  common::Error GetInner(const raw_key_t& key, raw_value_t* ret_val) WARN_UNUSED_RESULT;
  common::Error DelInner(const raw_key_t& key) WARN_UNUSED_RESULT;

  virtual common::Error ScanImpl(cursor_t cursor_in,
                                 const pattern_t& pattern,
                                 keys_limit_t count_keys,
                                 raw_keys_t* keys_out,
                                 cursor_t* cursor_out) override;
  virtual common::Error KeysImpl(const raw_key_t& key_start,
                                 const raw_key_t& key_end,
                                 keys_limit_t limit,
                                 raw_keys_t* ret) override;
  virtual common::Error DBkcountImpl(keys_limit_t* size) override;
  virtual common::Error FlushDBImpl() override;
  virtual common::Error CreateDBImpl(const db_name_t& name, IDataBaseInfo** info) override;
  virtual common::Error RemoveDBImpl(const db_name_t& name, IDataBaseInfo** info) override;
  virtual common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) override;
  virtual common::Error SetImpl(const NDbKValue& key, NDbKValue* added_key) override;
  virtual common::Error GetImpl(const NKey& key, NDbKValue* loaded_key) override;
  virtual common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) override;
  virtual common::Error RenameImpl(const NKey& key, const key_t& new_key) override;
  virtual common::Error QuitImpl() override;
  virtual common::Error ConfigGetDatabasesImpl(db_names_t* dbs) override;
};

}  // namespace forestdb
}  // namespace core
}  // namespace fastonosql
