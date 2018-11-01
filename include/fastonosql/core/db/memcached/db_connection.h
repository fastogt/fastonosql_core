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

#include <fastonosql/core/cdb_connection.h>  // for CDBConnection

#include <fastonosql/core/db/memcached/config.h>
#include <fastonosql/core/db/memcached/server_info.h>

struct memcached_st;  // lines 37-37

namespace fastonosql {
namespace core {
namespace memcached {

typedef memcached_st NativeConnection;

common::Error CreateConnection(const Config& config, NativeConnection** context);
common::Error TestConnection(const Config& config);

class DBConnection : public CDBConnection<NativeConnection, Config, MEMCACHED> {
 public:
  typedef CDBConnection<NativeConnection, Config, MEMCACHED> base_class;
  explicit DBConnection(CDBConnectionClient* client);

  common::Error Info(const std::string& args, ServerInfo::Stats* statsout) WARN_UNUSED_RESULT;

  common::Error AddIfNotExist(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags)
      WARN_UNUSED_RESULT;
  common::Error Replace(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags)
      WARN_UNUSED_RESULT;
  common::Error Append(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags) WARN_UNUSED_RESULT;
  common::Error Prepend(const NKey& key, const raw_value_t& value, time_t expiration, uint32_t flags)
      WARN_UNUSED_RESULT;
  common::Error Incr(const NKey& key, uint32_t value, uint64_t* result) WARN_UNUSED_RESULT;
  common::Error Decr(const NKey& key, uint32_t value, uint64_t* result) WARN_UNUSED_RESULT;

 private:
  common::Error CheckResultCommand(const std::string& cmd, int err) WARN_UNUSED_RESULT;

  common::Error TTLInner(const raw_key_t& key, ttl_t* expiration) WARN_UNUSED_RESULT;
  common::Error ExpireInner(const raw_key_t& key, ttl_t expiration) WARN_UNUSED_RESULT;

  common::Error DelInner(const raw_key_t& key, time_t expiration) WARN_UNUSED_RESULT;
  common::Error GetInner(const raw_key_t& key, raw_value_t* ret_val) WARN_UNUSED_RESULT;
  common::Error SetInner(const raw_key_t& key, const raw_value_t& value, time_t expiration, uint32_t flags)
      WARN_UNUSED_RESULT;

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
  virtual common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) override;
  virtual common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) override;
  virtual common::Error GetImpl(const NKey& key, NDbKValue* loaded_key) override;
  virtual common::Error SetImpl(const NDbKValue& key) override;
  virtual common::Error RenameImpl(const NKey& key, const key_t& new_key) override;
  virtual common::Error SetTTLImpl(const NKey& key, ttl_t ttl) override;
  virtual common::Error GetTTLImpl(const NKey& key, ttl_t* ttl) override;
  virtual common::Error QuitImpl() override;
  virtual common::Error ConfigGetDatabasesImpl(db_names_t* dbs) override;

  ServerInfo::Stats current_info_;
};

}  // namespace memcached
}  // namespace core
}  // namespace fastonosql
