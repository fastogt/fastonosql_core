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
  typedef std::string raw_key_t;
  typedef std::string raw_value_t;

  virtual bool IsAuthenticated() const override;

  virtual common::Error Connect(const config_t& config) override WARN_UNUSED_RESULT;
  virtual common::Error Disconnect() override WARN_UNUSED_RESULT;

  common::Error Info(const command_buffer_t& args, ServerInfo::Stats* statsout) WARN_UNUSED_RESULT;
  common::Error Auth(const std::string& password) WARN_UNUSED_RESULT;
  common::Error Setx(const raw_key_t& key, const raw_value_t& value, ttl_t ttl) WARN_UNUSED_RESULT;
  common::Error Incr(const raw_key_t& key, int64_t incrby, int64_t* out) WARN_UNUSED_RESULT;
  common::Error ScanSsdb(const raw_key_t& key_start,
                         const raw_key_t& key_end,
                         uint64_t limit,
                         std::vector<raw_key_t>* out) WARN_UNUSED_RESULT;
  common::Error Rscan(const raw_key_t& key_start, const raw_key_t& key_end, uint64_t limit, std::vector<raw_key_t>* out)
      WARN_UNUSED_RESULT;
  common::Error MultiGet(const std::vector<std::string>& keys, std::vector<std::string>* out);
  common::Error MultiSet(const std::map<std::string, std::string>& kvs) WARN_UNUSED_RESULT;
  common::Error MultiDel(const std::vector<std::string>& keys) WARN_UNUSED_RESULT;
  common::Error Hget(const raw_key_t& name, const raw_key_t& key, std::string* out) WARN_UNUSED_RESULT;
  common::Error Hgetall(const raw_key_t& name, std::vector<raw_key_t>* out) WARN_UNUSED_RESULT;
  common::Error Hset(const raw_key_t& name, const std::string& key, const std::string& val) WARN_UNUSED_RESULT;
  common::Error Hdel(const raw_key_t& name, const std::string& key) WARN_UNUSED_RESULT;
  common::Error Hincr(const raw_key_t& name, const std::string& key, int64_t incrby, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Hsize(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Hclear(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Hkeys(const raw_key_t& name,
                      const raw_key_t& key_start,
                      const raw_key_t& key_end,
                      uint64_t limit,
                      std::vector<raw_key_t>* out) WARN_UNUSED_RESULT;
  common::Error Hscan(const raw_key_t& name,
                      const raw_key_t& key_start,
                      const raw_key_t& key_end,
                      uint64_t limit,
                      std::vector<raw_key_t>* out) WARN_UNUSED_RESULT;
  common::Error Hrscan(const raw_key_t& name,
                       const raw_key_t& key_start,
                       const raw_key_t& key_end,
                       uint64_t limit,
                       std::vector<raw_key_t>* out) WARN_UNUSED_RESULT;
  common::Error MultiHget(const raw_key_t& name,
                          const std::vector<raw_key_t>& keys,
                          std::vector<std::string>* out) WARN_UNUSED_RESULT;
  common::Error MultiHset(const raw_key_t& name, const std::map<raw_key_t, raw_value_t>& keys) WARN_UNUSED_RESULT;
  common::Error Zget(const raw_key_t& name, const std::string& key, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zset(const raw_key_t& name, const std::string& key, int64_t score) WARN_UNUSED_RESULT;
  common::Error Zdel(const raw_key_t& name, const std::string& key) WARN_UNUSED_RESULT;
  common::Error Zincr(const raw_key_t& name, const std::string& key, int64_t incrby, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zsize(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zclear(const raw_key_t& name, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zrank(const raw_key_t& name, const std::string& key, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zrrank(const raw_key_t& name, const std::string& key, int64_t* out) WARN_UNUSED_RESULT;
  common::Error Zrange(const raw_key_t& name, uint64_t offset, uint64_t limit, std::vector<std::string>* out)
      WARN_UNUSED_RESULT;
  common::Error Zrrange(const std::string& name, uint64_t offset, uint64_t limit, std::vector<std::string>* out)
      WARN_UNUSED_RESULT;
  common::Error Zkeys(const std::string& name,
                      const std::string& key_start,
                      int64_t* score_start,
                      int64_t* score_end,
                      uint64_t limit,
                      std::vector<std::string>* out) WARN_UNUSED_RESULT;
  common::Error Zscan(const std::string& name,
                      const std::string& key_start,
                      int64_t* score_start,
                      int64_t* score_end,
                      uint64_t limit,
                      std::vector<std::string>* out) WARN_UNUSED_RESULT;
  common::Error Zrscan(const std::string& name,
                       const std::string& key_start,
                       int64_t* score_start,
                       int64_t* score_end,
                       uint64_t limit,
                       std::vector<std::string>* out) WARN_UNUSED_RESULT;
  common::Error MultiZget(const std::string& name,
                          const std::vector<std::string>& keys,
                          std::vector<std::string>* out) WARN_UNUSED_RESULT;
  common::Error MultiZset(const std::string& name, const std::map<std::string, int64_t>& kss);
  common::Error MultiZdel(const std::string& name, const std::vector<std::string>& keys) WARN_UNUSED_RESULT;
  common::Error Qpush(const std::string& name, const std::string& item) WARN_UNUSED_RESULT;
  common::Error Qpop(const std::string& name, std::string* out) WARN_UNUSED_RESULT;
  common::Error Qslice(const std::string& name, int64_t begin, int64_t end, std::vector<std::string>* out)
      WARN_UNUSED_RESULT;
  common::Error Qclear(const std::string& name, int64_t* ret) WARN_UNUSED_RESULT;
  common::Error DBsize(int64_t* size) WARN_UNUSED_RESULT;

  common::Error Expire(key_t key, ttl_t ttl) WARN_UNUSED_RESULT;
  common::Error TTL(key_t key, ttl_t* ttl) WARN_UNUSED_RESULT;

 private:
  common::Error SetInner(const raw_key_t& key, const raw_value_t& value) WARN_UNUSED_RESULT;
  common::Error GetInner(const raw_key_t& key, raw_value_t* ret_val) WARN_UNUSED_RESULT;
  common::Error DelInner(const raw_key_t& key) WARN_UNUSED_RESULT;

  virtual common::Error ConfigGetDatabasesImpl(std::vector<std::string>* dbs) override;
  virtual common::Error ScanImpl(cursor_t cursor_in,
                                 const pattern_t& pattern,
                                 keys_limit_t count_keys,
                                 raw_keys_t* keys_out,
                                 cursor_t* cursor_out) override;
  virtual common::Error KeysImpl(const command_buffer_t& key_start,
                                 const command_buffer_t& key_end,
                                 keys_limit_t limit,
                                 raw_keys_t* ret) override;
  virtual common::Error DBkcountImpl(keys_limit_t* size) override;
  virtual common::Error FlushDBImpl() override;
  virtual common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) override;
  virtual common::Error SetImpl(const NDbKValue& key, NDbKValue* added_key) override;
  virtual common::Error GetImpl(const NKey& key, NDbKValue* loaded_key) override;
  virtual common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) override;
  virtual common::Error RenameImpl(const NKey& key, const key_t& new_key) override;
  virtual common::Error SetTTLImpl(const NKey& key, ttl_t ttl) override;
  virtual common::Error GetTTLImpl(const NKey& key, ttl_t* ttl) override;
  virtual common::Error QuitImpl() override;

 private:
  common::Error CheckResultCommand(const std::string& cmd, const ::ssdb::Status& err) WARN_UNUSED_RESULT;
  bool is_auth_;
};

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
