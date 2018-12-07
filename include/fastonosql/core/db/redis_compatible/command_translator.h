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

#include <fastonosql/core/icommand_translator.h>  // for ICommandTranslator

#define REDIS_GET_PTTL_COMMAND "PTTL"
#define REDIS_CHANGE_PTTL_COMMAND "PEXPIRE"

namespace fastonosql {
namespace core {
namespace redis_compatible {

class CommandTranslator : public ICommandTranslator {
 public:
  explicit CommandTranslator(const std::vector<CommandHolder>& commands);

  common::Error Zrange(const NKey& key, int start, int stop, bool withscores, command_buffer_t* cmdstring)
      WARN_UNUSED_RESULT;

  common::Error Hgetall(const NKey& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error Mget(const std::vector<NKey>& keys, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error Mset(const std::vector<NDbKValue>& keys, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error MsetNX(const std::vector<NDbKValue>& keys, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  common::Error Smembers(const NKey& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  common::Error Lrange(const NKey& key, int start, int stop, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  common::Error Append(const NKey& key, const NValue& value, command_buffer_t* cmdstring);

  common::Error SetEx(const NDbKValue& key, ttl_t ttl, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error SetEx(const trans_key_t& key, const trans_value_t& value, ttl_t ttl, command_buffer_t* cmdstring)
      WARN_UNUSED_RESULT;

  common::Error SetNX(const NDbKValue& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  common::Error Decr(const NKey& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error DecrBy(const NKey& key, int inc, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  common::Error Incr(const NKey& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error IncrBy(const NKey& key, int inc, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error IncrByFloat(const NKey& key, double inc, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  common::Error PExpire(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;
  common::Error PTTL(const NKey& key, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;

  common::Error Lpush(const NKey& key, const NValue& arr, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;
  common::Error Rpush(const NKey& key, const NValue& arr, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;
  common::Error Sadd(const NKey& key, const NValue& set, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;
  common::Error Zadd(const NKey& key, const NValue& zset, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;
  common::Error Hmset(const NKey& key, const NValue& hash, command_buffer_t* cmdstring) const WARN_UNUSED_RESULT;

 private:
  common::Error CreateKeyCommandImpl(const NDbKValue& key, command_buffer_t* cmdstring) const override;
  common::Error LoadKeyCommandImpl(const NKey& key,
                                   common::Value::Type type,
                                   command_buffer_t* cmdstring) const override;
  common::Error DeleteKeyCommandImpl(const NKey& key, command_buffer_t* cmdstring) const override;
  common::Error RenameKeyCommandImpl(const NKey& key,
                                     const trans_key_t& new_name,
                                     command_buffer_t* cmdstring) const override;
  common::Error ChangeKeyTTLCommandImpl(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const override;
  common::Error LoadKeyTTLCommandImpl(const NKey& key, command_buffer_t* cmdstring) const override;

  bool IsLoadKeyCommandImpl(const CommandInfo& cmd) const override;

  common::Error PublishCommandImpl(const trans_ps_channel_t& channel,
                                   const std::string& message,
                                   command_buffer_t* cmdstring) const override;
  common::Error SubscribeCommandImpl(const trans_ps_channel_t& channel, command_buffer_t* cmdstring) const override;
};

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
