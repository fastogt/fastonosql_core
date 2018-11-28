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

#include <map>
#include <string>
#include <vector>

#include <fastonosql/core/cdb_connection.h>

#include <fastonosql/core/db/ssdb/config.h>
#include <fastonosql/core/db/ssdb/server_info.h>

namespace ssdb {
class Client;
class Status;
}  // namespace ssdb

namespace fastonosql {
namespace core {
namespace ssdb {

typedef ::ssdb::Client NativeConnection;

common::Error CreateConnection(const Config& config, NativeConnection** context);
common::Error TestConnection(const Config& config);

class DBConnection : public CDBConnection<NativeConnection, Config, SSDB> {
 public:
  typedef CDBConnection<NativeConnection, Config, SSDB> base_class;
  explicit DBConnection(CDBConnectionClient* client);

  bool IsAuthenticated() const override;

  common::Error Connect(const config_t& config) override WARN_UNUSED_RESULT;
  common::Error Disconnect() override WARN_UNUSED_RESULT;

  common::Error Info(const command_buffer_t& args, ServerInfo::Stats* statsout) WARN_UNUSED_RESULT;
  common::Error Auth(const std::string& password) WARN_UNUSED_RESULT;
  common::Error Setx(const raw_key_t& key, const raw_value_t& value, ttl_t ttl) WARN_UNUSED_RESULT;
  common::Error Incr(const raw_key_t& key, int64_t incrby, int64_t* out) WARN_UNUSED_RESULT;
  common::Error ScanSsdb(const raw_key_t& key_start, const raw_key_t& key_end, uint64_t limit, raw_keys_t* out)
      WARN_UNUSED_RESULT;
  common::Error Rscan(const raw_key_t& key_start, const raw_key_t& key_end, uint64_t limit, raw_keys_t* out)
      WARN_UNUSED_RESULT;
  common::Error MultiGet(const raw_keys_t& keys, raw_values_t* out);
  common::Error MultiSet(const std::map<raw_key_t, raw_value_t>& kvs) WARN_UNUSED_RESULT;
  common::Error MultiDel(const raw_keys_t& keys) WARN_UNUSED_RESULT;
  common::Error Hget(const raw_key_t& name, const raw_key_t& key, raw_value_t* out) WARN_UNUSED_RESULT;
  common::Error Hgetall(const raw_key_t& name, raw_keys_t* out) WARN_UNUSED_RESULT;
  common::Error Hset(const raw_key_t& name, const raw_key_t& key, const raw_value_t& val) WARN_UNUSED_RESULT;
  common::Error Hdel(const raw_key_t& name, const raw_key_t& key) WARN_UNUSED_RESULT;
  common::Error Hincr(const raw_key_t& name, const raw_key_t& key, int64_t incrby, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Hsize(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Hclear(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Hkeys(const raw_key_t& name,
                      const raw_key_t& key_start,
                      const raw_key_t& key_end,
                      uint64_t limit,
                      raw_keys_t* out) WARN_UNUSED_RESULT;
  common::Error Hscan(const raw_key_t& name,
                      const raw_key_t& key_start,
                      const raw_key_t& key_end,
                      uint64_t limit,
                      raw_keys_t* out) WARN_UNUSED_RESULT;
  common::Error Hrscan(const raw_key_t& name,
                       const raw_key_t& key_start,
                       const raw_key_t& key_end,
                       uint64_t limit,
                       raw_keys_t* out) WARN_UNUSED_RESULT;
  common::Error MultiHget(const raw_key_t& name, const raw_keys_t& keys, raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error MultiHset(const raw_key_t& name, const std::map<raw_key_t, raw_value_t>& keys) WARN_UNUSED_RESULT;
  common::Error Zget(const raw_key_t& name, const raw_key_t& key, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zset(const raw_key_t& name, const raw_key_t& key, int64_t score) WARN_UNUSED_RESULT;
  common::Error Zdel(const raw_key_t& name, const raw_key_t& key) WARN_UNUSED_RESULT;
  common::Error Zincr(const raw_key_t& name, const raw_key_t& key, int64_t incrby, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zsize(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zclear(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zrank(const raw_key_t& name, const raw_key_t& key, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zrrank(const raw_key_t& name, const raw_key_t& key, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zrange(const raw_key_t& name, uint64_t offset, uint64_t limit, raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error Zrrange(const raw_key_t& name, uint64_t offset, uint64_t limit, raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error Zkeys(const raw_key_t& name,
                      const raw_key_t& key_start,
                      int64_t* score_start,
                      int64_t* score_end,
                      uint64_t limit,
                      raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error Zscan(const raw_key_t& name,
                      const raw_key_t& key_start,
                      int64_t* score_start,
                      int64_t* score_end,
                      uint64_t limit,
                      raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error Zrscan(const raw_key_t& name,
                       const raw_key_t& key_start,
                       int64_t* score_start,
                       int64_t* score_end,
                       uint64_t limit,
                       raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error MultiZget(const raw_key_t& name, const raw_keys_t& keys, raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error MultiZset(const raw_key_t& name, const std::map<raw_key_t, int64_t>& kss);
  common::Error MultiZdel(const raw_key_t& name, const std::vector<raw_key_t>& keys) WARN_UNUSED_RESULT;
  common::Error Qpush(const NKey& key, NValue arr, int64_t* list_len) WARN_UNUSED_RESULT;
  common::Error Qpop(const raw_key_t& name, raw_value_t* out) WARN_UNUSED_RESULT;
  common::Error Qrange(const NKey& key, int start, int stop, NDbKValue* loaded_key) WARN_UNUSED_RESULT;
  common::Error Qslice(const raw_key_t& name, int64_t begin, int64_t end, raw_values_t* out) WARN_UNUSED_RESULT;
  common::Error Qclear(const raw_key_t& name, int64_t* ret) WARN_UNUSED_RESULT;
  common::Error DBsize(int64_t* size) WARN_UNUSED_RESULT;

 private:
  common::Error ExpireInner(const raw_key_t& key, ttl_t ttl) WARN_UNUSED_RESULT;
  common::Error TTLInner(const raw_key_t& key, ttl_t* ttl) WARN_UNUSED_RESULT;

  common::Error SetInner(const raw_key_t& key, const raw_value_t& value) WARN_UNUSED_RESULT;
  common::Error GetInner(const raw_key_t& key, raw_value_t* ret_val) WARN_UNUSED_RESULT;
  common::Error DelInner(const raw_key_t& key) WARN_UNUSED_RESULT;

  common::Error ConfigGetDatabasesImpl(db_names_t* dbs) override;
  common::Error ScanImpl(cursor_t cursor_in,
                         const pattern_t& pattern,
                         keys_limit_t count_keys,
                         raw_keys_t* keys_out,
                         cursor_t* cursor_out) override;
  common::Error KeysImpl(const raw_key_t& key_start,
                         const raw_key_t& key_end,
                         keys_limit_t limit,
                         raw_keys_t* ret) override;
  common::Error DBKeysCountImpl(keys_limit_t* size) override;
  common::Error FlushDBImpl() override;
  common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) override;
  common::Error SetImpl(const NDbKValue& key) override;
  common::Error GetImpl(const NKey& key, NDbKValue* loaded_key) override;
  common::Error QrangeImpl(const NKey& key, int start, int stop, NDbKValue* loaded_key);
#if 0
  common::Error GetTypeImpl(const NKey& key, readable_string_t* type) override;
#endif
  common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) override;
  common::Error RenameImpl(const NKey& key, const nkey_t& new_key) override;
  common::Error SetTTLImpl(const NKey& key, ttl_t ttl) override;
  common::Error GetTTLImpl(const NKey& key, ttl_t* ttl) override;
  common::Error QuitImpl() override;

 private:
  common::Error CheckResultCommand(const std::string& cmd, const ::ssdb::Status& err) WARN_UNUSED_RESULT;
  bool is_auth_;
};

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
