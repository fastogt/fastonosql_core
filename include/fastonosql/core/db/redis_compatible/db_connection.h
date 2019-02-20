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

#include <string>
#include <vector>

#include <common/convert2string.h>

#include <fastonosql/core/cdb_connection.h>

#include <fastonosql/core/db/redis_compatible/config.h>

#include <fastonosql/core/global.h>
#include <fastonosql/core/ssh_info.h>

#if defined(PRO_VERSION)
#include <fastonosql/core/cluster/cluster_discovery_info.h>
#include <fastonosql/core/sentinel/sentinel_discovery_info.h>
#endif

struct redisContext;
struct redisReply;

namespace fastonosql {
namespace core {
namespace redis_compatible {

class CommandTranslator;

typedef redisContext NativeConnection;

bool ConvertFromString(const readable_string_t& value, common::Value::Type* out);
bool ConvertFromType(common::Value::Type type, readable_string_t* out);

const char* GetHiredisVersion();

common::Error CreateConnection(const Config& config, const SSHInfo& sinfo, NativeConnection** context);
common::Error TestConnection(const Config& config, const SSHInfo& sinfo);

common::Error PrintRedisContextError(NativeConnection* context);
common::Error ValueFromReplay(redisReply* reply, common::Value** out);
common::Error ExecRedisCommand(NativeConnection* context,
                               size_t argc,
                               const char** argv,
                               const size_t* argvlen,
                               redisReply** out_reply);
common::Error ExecRedisCommand(NativeConnection* context, const commands_args_t& argv, redisReply** out_reply);
common::Error ExecRedisCommand(NativeConnection* context, const command_buffer_t& command, redisReply** out_reply);
common::Error AuthContext(NativeConnection* context, const command_buffer_t& password);

template <typename Config, ConnectionType connection_type>
class DBConnection : public CDBConnection<NativeConnection, Config, connection_type> {
 public:
  typedef long long redis_int_t;
  typedef std::shared_ptr<CommandTranslator> redis_translator_t;
  typedef CDBConnection<NativeConnection, Config, connection_type> base_class;
  typedef typename base_class::config_t config_t;

  enum { invalid_db_num = -1 };

  common::Error Connect(const config_t& config) override WARN_UNUSED_RESULT;
  common::Error Disconnect() override WARN_UNUSED_RESULT;

  db_name_t GetCurrentDBName() const override;

  bool IsAuthenticated() const override;

  common::Error SetClientName(const std::string& name) WARN_UNUSED_RESULT;

  common::Error CommonExec(const commands_args_t& argv, FastoObject* out) WARN_UNUSED_RESULT;

  common::Error Auth(const command_buffer_t& password) WARN_UNUSED_RESULT;

  common::Error SlaveMode(FastoObject* out) WARN_UNUSED_RESULT;

  common::Error Monitor(const commands_args_t& argv,
                        FastoObject* out) WARN_UNUSED_RESULT;  // interrupt
  common::Error Subscribe(const commands_args_t& argv,
                          FastoObject* out) WARN_UNUSED_RESULT;  // interrupt

  common::Error Lpush(const NKey& key, NValue arr, redis_int_t* list_len) WARN_UNUSED_RESULT;
  common::Error Rpush(const NKey& key, NValue arr, redis_int_t* list_len) WARN_UNUSED_RESULT;
  common::Error LFastoSet(const NKey& key, NValue arr, redis_int_t* list_len) WARN_UNUSED_RESULT;
  common::Error Lrange(const NKey& key, int start, int stop, NDbKValue* loaded_key) WARN_UNUSED_RESULT;

  common::Error Mget(const std::vector<NKey>& keys, std::vector<NDbKValue>* loaded_keys) WARN_UNUSED_RESULT;
  common::Error Mset(const std::vector<NDbKValue>& keys) WARN_UNUSED_RESULT;
  common::Error MsetNX(const std::vector<NDbKValue>& keys, redis_int_t* result) WARN_UNUSED_RESULT;

  common::Error Append(const NKey& key, const NValue& val, redis_int_t* new_string_len) WARN_UNUSED_RESULT;

  common::Error SetEx(const NDbKValue& key, ttl_t ttl) WARN_UNUSED_RESULT;
  common::Error SetNX(const NDbKValue& key, redis_int_t* result) WARN_UNUSED_RESULT;

  common::Error Decr(const NKey& key, redis_int_t* decr) WARN_UNUSED_RESULT;
  common::Error DecrBy(const NKey& key, int inc, redis_int_t* decr) WARN_UNUSED_RESULT;

  common::Error Incr(const NKey& key, redis_int_t* incr) WARN_UNUSED_RESULT;
  common::Error IncrBy(const NKey& key, int inc, redis_int_t* incr) WARN_UNUSED_RESULT;
  common::Error IncrByFloat(const NKey& key, double inc, command_buffer_t* str_incr) WARN_UNUSED_RESULT;

  common::Error PExpire(const NKey& key,
                        ttl_t ttl) WARN_UNUSED_RESULT;  // PEXPIRE works differently than in redis protocol
  common::Error PTTL(const NKey& key, pttl_t* ttl) WARN_UNUSED_RESULT;

  common::Error Sadd(const NKey& key, NValue set, redis_int_t* added) WARN_UNUSED_RESULT;
  common::Error SFastoSet(const NKey& key, NValue set, redis_int_t* set_added_len) WARN_UNUSED_RESULT;
  common::Error Smembers(const NKey& key, NDbKValue* loaded_key) WARN_UNUSED_RESULT;

  common::Error Zadd(const NKey& key, NValue scores, redis_int_t* added) WARN_UNUSED_RESULT;
  common::Error ZFastoSet(const NKey& key, NValue set, redis_int_t* score_added_len) WARN_UNUSED_RESULT;
  common::Error Zrange(const NKey& key, int start, int stop, bool withscores, NDbKValue* loaded_key) WARN_UNUSED_RESULT;

  common::Error Hmset(const NKey& key, NValue hash) WARN_UNUSED_RESULT;
  common::Error HFastoSet(const NKey& key, NValue set) WARN_UNUSED_RESULT;
  common::Error Hgetall(const NKey& key, NDbKValue* loaded_key) WARN_UNUSED_RESULT;

  common::Error DBSize(keys_limit_t* size) WARN_UNUSED_RESULT;

  common::Error ExecuteAsPipeline(const std::vector<FastoObjectCommandIPtr>& cmds,
                                  void (*log_command_cb)(FastoObjectCommandIPtr)) WARN_UNUSED_RESULT;

  IDataBaseInfo* MakeDatabaseInfo(const db_name_t& name, bool is_default, size_t size) const override;

 protected:
  DBConnection(CDBConnectionClient* client, ICommandTranslator* translator)
      : base_class(client, translator), is_auth_(false), cur_db_(invalid_db_num) {}

  common::Error CliFormatReplyRaw(FastoObject* out, redisReply* r) WARN_UNUSED_RESULT;

  common::Error LrangeImpl(const NKey& key, int start, int stop, NDbKValue* loaded_key);                   // for list
  common::Error SmembersImpl(const NKey& key, NDbKValue* loaded_key);                                      // for set
  common::Error HgetallImpl(const NKey& key, NDbKValue* loaded_key);                                       // for hash
  common::Error ZrangeImpl(const NKey& key, int start, int stop, bool withscores, NDbKValue* loaded_key);  // for zset
  common::Error GetImpl(const NKey& key,
                        NDbKValue* loaded_key) override;  // GET works differently than in redis protocol
  common::Error GetTypeImpl(const NKey& key,
                            readable_string_t* type) override;  // TYPE works differently than in redis protocol

 private:
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
  common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) override;
  common::Error SetImpl(const NDbKValue& key) override;
  common::Error GetUniImpl(const NKey& key, NDbKValue* loaded_key) override;
  common::Error RenameImpl(const NKey& key, const nkey_t& new_key) override;
  common::Error SetTTLImpl(const NKey& key,
                           ttl_t ttl) override;  // EXPIRE works differently than in redis protocol
  common::Error GetTTLImpl(const NKey& key, ttl_t* ttl) override;
  common::Error QuitImpl() override;
  common::Error ConfigGetDatabasesImpl(db_names_t* dbs) override;

  common::Error CliReadReply(FastoObject* out) WARN_UNUSED_RESULT;
  common::Error SendSync(unsigned long long* payload) WARN_UNUSED_RESULT;

  bool is_auth_;
  int cur_db_;
};

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
