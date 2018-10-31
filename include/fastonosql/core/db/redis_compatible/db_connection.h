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

#include <common/convert2string.h>

#include <fastonosql/core/cdb_connection.h>  // for CDBConnection

#include <fastonosql/core/server/iserver_info.h>

#include <fastonosql/core/db/redis_compatible/command_translator.h>
#include <fastonosql/core/db/redis_compatible/config.h>

#include <fastonosql/core/global.h>
#include <fastonosql/core/ssh_info.h>

struct redisContext;
struct redisReply;

namespace fastonosql {
namespace core {
namespace redis_compatible {

typedef redisContext NativeConnection;

const char* GetHiredisVersion();

common::Error CreateConnection(const Config& config, const SSHInfo& sinfo, NativeConnection** context);
common::Error TestConnection(const Config& config, const SSHInfo& sinfo);

#if defined(PRO_VERSION)
common::Error DiscoveryClusterConnection(const Config& rconfig,
                                         const SSHInfo& sinfo,
                                         std::vector<ServerDiscoveryClusterInfoSPtr>* infos);
common::Error DiscoverySentinelConnection(const Config& rconfig,
                                          const SSHInfo& sinfo,
                                          std::vector<ServerDiscoverySentinelInfoSPtr>* infos);
#endif

common::Error PrintRedisContextError(NativeConnection* context);
common::Error ValueFromReplay(redisReply* r, common::Value** out);
common::Error ExecRedisCommand(NativeConnection* c,
                               size_t argc,
                               const char** argv,
                               const size_t* argvlen,
                               redisReply** out_reply);
common::Error ExecRedisCommand(NativeConnection* c, const commands_args_t& argv, redisReply** out_reply);
common::Error ExecRedisCommand(NativeConnection* c, const command_buffer_t& command, redisReply** out_reply);
common::Error AuthContext(NativeConnection* context, const command_buffer_t& password);

template <typename Config, ConnectionType connection_type>
class DBConnection : public CDBConnection<NativeConnection, Config, connection_type> {
 public:
  typedef std::shared_ptr<CommandTranslator> redis_translator_t;
  typedef CDBConnection<NativeConnection, Config, connection_type> base_class;
  typedef typename base_class::config_t config_t;

  enum { invalid_db_num = -1 };
  explicit DBConnection(CDBConnectionClient* client)
      : base_class(client, new CommandTranslator(base_class::GetCommands())),
        is_auth_(false),
        cur_db_(invalid_db_num) {}

  virtual common::Error Connect(const config_t& config) override;
  virtual common::Error Disconnect() override;

  virtual db_name_t GetCurrentDBName() const override;

  virtual bool IsAuthenticated() const override;

  common::Error CommonExec(const commands_args_t& argv, FastoObject* out) WARN_UNUSED_RESULT;

  common::Error Auth(const command_buffer_t& password) WARN_UNUSED_RESULT;

  common::Error SlaveMode(FastoObject* out) WARN_UNUSED_RESULT;

  common::Error Monitor(const commands_args_t& argv,
                        FastoObject* out) WARN_UNUSED_RESULT;  // interrupt
  common::Error Subscribe(const commands_args_t& argv,
                          FastoObject* out) WARN_UNUSED_RESULT;  // interrupt

  common::Error Lpush(const NKey& key, NValue arr, long long* list_len) WARN_UNUSED_RESULT;
  common::Error Rpush(const NKey& key, NValue arr, long long* list_len) WARN_UNUSED_RESULT;
  common::Error LfastoSet(const NKey& key, NValue arr, long long* list_len) WARN_UNUSED_RESULT;
  common::Error Lrange(const NKey& key, int start, int stop, NDbKValue* loaded_key) WARN_UNUSED_RESULT;

  common::Error Mget(const std::vector<NKey>& keys, std::vector<NDbKValue>* loaded_keys) WARN_UNUSED_RESULT;
  common::Error Mset(const std::vector<NDbKValue>& keys, std::vector<NDbKValue>* added_key) WARN_UNUSED_RESULT;
  common::Error MsetNX(const std::vector<NDbKValue>& keys, long long* result) WARN_UNUSED_RESULT;

  common::Error SetEx(const NDbKValue& key, ttl_t ttl) WARN_UNUSED_RESULT;
  common::Error SetNX(const NDbKValue& key, long long* result) WARN_UNUSED_RESULT;

  common::Error Decr(const NKey& key, long long* decr);
  common::Error DecrBy(const NKey& key, int inc, long long* decr);

  common::Error Incr(const NKey& key, long long* incr);
  common::Error IncrBy(const NKey& key, int inc, long long* incr);
  common::Error IncrByFloat(const NKey& key, double inc, command_buffer_t* str_incr);

  common::Error PExpire(const NKey& key,
                        ttl_t ttl) WARN_UNUSED_RESULT;  // PEXPIRE works differently than in redis protocol
  common::Error PTTL(const NKey& key, pttl_t* ttl) WARN_UNUSED_RESULT;

  common::Error Sadd(const NKey& key, NValue set, long long* added);
  common::Error Smembers(const NKey& key, NDbKValue* loaded_key);

  common::Error Zadd(const NKey& key, NValue scores, long long* added);
  common::Error Zrange(const NKey& key, int start, int stop, bool withscores, NDbKValue* loaded_key);

  common::Error Hmset(const NKey& key, NValue hash);
  common::Error Hgetall(const NKey& key, NDbKValue* loaded_key);

  common::Error ExecuteAsPipeline(const std::vector<FastoObjectCommandIPtr>& cmds,
                                  void (*log_command_cb)(FastoObjectCommandIPtr)) WARN_UNUSED_RESULT;

 protected:
  common::Error CliFormatReplyRaw(FastoObject* out, redisReply* r) WARN_UNUSED_RESULT;

 private:
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
  virtual common::Error SetImpl(const NDbKValue& key, NDbKValue* added_key) override;
  virtual common::Error GetImpl(const NKey& key,
                                NDbKValue* loaded_key) override;  // GET works differently than in redis protocol
  virtual common::Error RenameImpl(const NKey& key, const key_t& new_key) override;
  virtual common::Error SetTTLImpl(const NKey& key,
                                   ttl_t ttl) override;  // EXPIRE works differently than in redis protocol
  virtual common::Error GetTTLImpl(const NKey& key, ttl_t* ttl) override;
  virtual common::Error QuitImpl() override;
  virtual common::Error ConfigGetDatabasesImpl(db_names_t* dbs) override;

  common::Error CliReadReply(FastoObject* out) WARN_UNUSED_RESULT;
  common::Error SendSync(unsigned long long* payload) WARN_UNUSED_RESULT;

  bool is_auth_;
  int cur_db_;
};

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
