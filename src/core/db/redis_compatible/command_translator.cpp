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

#include <fastonosql/core/db/redis_compatible/command_translator.h>

#include <common/convert2string.h>

#include <fastonosql/core/connection_types.h>
#include <fastonosql/core/value.h>

#include "core/db/redis/internal/modules.h"

#define REDIS_SET_KEY_COMMAND DB_SET_KEY_COMMAND
#define REDIS_SET_KEY_ARRAY_COMMAND "LFASTOSET"
#define REDIS_SET_KEY_SET_COMMAND "SFASTOSET"
#define REDIS_SET_KEY_ZSET_COMMAND "ZFASTOSET"
#define REDIS_SET_KEY_HASH_COMMAND "HFASTOSET"
#define REDIS_SET_KEY_STREAM_COMMAND "XFASTOSET"
#define REDIS_SET_KEY_JSON_COMMAND REDIS_JSON_MODULE_COMMAND("SET")

#define REDIS_GET_KEY_COMMAND DB_GET_KEY_COMMAND
#define REDIS_GET_KEY_ARRAY_COMMAND "LRANGE"
#define REDIS_GET_KEY_SET_COMMAND "SMEMBERS"
#define REDIS_GET_KEY_ZSET_COMMAND "ZRANGE"
#define REDIS_GET_KEY_HASH_COMMAND "HGETALL"
#define REDIS_GET_KEY_STREAM_COMMAND "XRANGE"
#define REDIS_GET_KEY_JSON_COMMAND REDIS_JSON_MODULE_COMMAND("GET")

//
#define REDIS_SADD_COMMAND "SADD"
#define REDIS_LPUSH_COMMAND "LPUSH"
#define REDIS_RPUSH_COMMAND "RPUSH"
#define REDIS_ZADD_COMMAND "ZADD"
#define REDIS_HMSET_COMMAND "HMSET"

//
#define REDIS_INCR_KEY_COMMAND "INCR"
#define REDIS_DECR_KEY_COMMAND "DECR"
#define REDIS_INCRBY_KEY_COMMAND "INCRBY"
#define REDIS_DECRBY_KEY_COMMAND "DECRBY"
#define REDIS_INCRBYFLOAT_KEY_COMMAND "INCRBYFLOAT"
// #define REDIS_DECRBYFLOAT_KEY_STREAM_COMMAND "DECRBYFLOAT"

#define REDIS_DELETE_KEY_COMMAND DB_DELETE_KEY_COMMAND
#define REDIS_RENAME_KEY_COMMAND DB_RENAME_KEY_COMMAND
#define REDIS_CHANGE_TTL_COMMAND DB_SET_TTL_COMMAND
#define REDIS_PERSIST_KEY_COMMAND "PERSIST"

#define REDIS_GET_TTL_COMMAND DB_GET_TTL_COMMAND
#define REDIS_PUBLISH_COMMAND DB_PUBLISH_COMMAND
#define REDIS_SUBSCRIBE_COMMAND DB_SUBSCRIBE_COMMAND

#define REDIS_ZRANGE "ZRANGE"

#define REDIS_MGET "MGET"
#define REDIS_MSETNX "MSETNX"
#define REDIS_MSET "MSET"
#define REDIS_HGETALL "HGETALL"

#define REDIS_SMEMBERS "SMEMBERS"

#define REDIS_LRANGE "LRANGE"

#define REDIS_APPEND "APPEND"
#define REDIS_SETEX "SETEX"
#define REDIS_SETNX "SETNX"

#define REDIS_DECR "DECR"
#define REDIS_DECRBY "DECRBY"

#define REDIS_INCR "INCR"
#define REDIS_INCRBY "INCRBY"
#define REDIS_INCRBYFLOAT "INCRBYFLOAT"

namespace fastonosql {
namespace core {
namespace redis_compatible {

CommandTranslator::CommandTranslator(const std::vector<CommandHolder>& commands) : ICommandTranslator(commands) {}

common::Error CommandTranslator::Zrange(const NKey& key,
                                        int start,
                                        int stop,
                                        bool withscores,
                                        command_buffer_t* cmdstring) {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_ZRANGE SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(start) << SPACE_STR
     << common::ConvertToBytes(stop);
  if (withscores) {
    wr << " WITHSCORES";
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Hgetall(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_HGETALL SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Mset(const std::vector<NDbKValue>& keys, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_MSET;
  for (size_t i = 0; i < keys.size(); ++i) {
    const NKey key = keys[i].GetKey();
    const auto key_str = key.GetKey();
    const NValue value = keys[i].GetValue();

    wr << SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  }
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::MsetNX(const std::vector<NDbKValue>& keys, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_MSETNX;
  for (size_t i = 0; i < keys.size(); ++i) {
    const NKey key = keys[i].GetKey();
    const auto key_str = key.GetKey();
    const NValue value = keys[i].GetValue();

    wr << SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  }
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Mget(const std::vector<NKey>& keys, command_buffer_t* cmdstring) {
  if (keys.empty() || !cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_MGET;
  for (size_t i = 0; i < keys.size(); ++i) {
    const auto key_str = keys[i].GetKey();
    wr << SPACE_STR << key_str.GetForCommandLine();
  }
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Smembers(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_SMEMBERS SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Lrange(const NKey& key, int start, int stop, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_LRANGE SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(start) << SPACE_STR
     << common::ConvertToBytes(stop);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Append(const NKey& key, const NValue& value, command_buffer_t* cmdstring) {
  if (!cmdstring || !value) {
    return common::make_error_inval();
  }

  const nkey_t key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_APPEND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::SetEx(const NDbKValue& key, ttl_t ttl, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const NKey cur = key.GetKey();
  const nkey_t key_str = cur.GetKey();
  const NValue value = key.GetValue();

  return SetEx(key_str, value.GetForCommandLine(), ttl, cmdstring);
}

common::Error CommandTranslator::SetEx(const trans_key_t& key,
                                       const trans_value_t& value,
                                       ttl_t ttl,
                                       command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_SETEX SPACE_STR << key.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(ttl) << SPACE_STR
     << value;
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::SetNX(const NDbKValue& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();

  command_buffer_writer_t wr;
  wr << REDIS_SETNX SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Decr(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_DECR SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::DecrBy(const NKey& key, int inc, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_DECRBY SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(inc);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Incr(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_INCR SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::IncrBy(const NKey& key, int inc, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_INCRBY SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(inc);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::IncrByFloat(const NKey& key, double inc, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_INCRBYFLOAT SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(inc);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::PExpire(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  if (ttl == NO_TTL) {
    wr << REDIS_PERSIST_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  } else {
    wr << REDIS_CHANGE_PTTL_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR
       << common::ConvertToBytes(ttl);
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::PTTL(const NKey& key, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_GET_PTTL_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Lpush(const NKey& key, const NValue& arr, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_LPUSH_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << arr.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Rpush(const NKey& key, const NValue& arr, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_RPUSH_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << arr.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Sadd(const NKey& key, const NValue& set, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_SADD_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << set.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Zadd(const NKey& key, const NValue& zset, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_ZADD_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << zset.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Hmset(const NKey& key, const NValue& hash, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << REDIS_HMSET_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << hash.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::CreateKeyCommandImpl(const NDbKValue& key, command_buffer_t* cmdstring) const {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();

  command_buffer_writer_t wr;
  common::Value::Type type = key.GetType();

  if (type == common::Value::TYPE_ARRAY) {
    wr << REDIS_SET_KEY_ARRAY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR
       << value.GetForCommandLine();
  } else if (type == common::Value::TYPE_SET) {
    wr << REDIS_SET_KEY_SET_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  } else if (type == common::Value::TYPE_ZSET) {
    wr << REDIS_SET_KEY_ZSET_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  } else if (type == common::Value::TYPE_HASH) {
    // HMSET gameConfig:1:1 tile "note3" RY "1920" RX "1080" id 1
    wr << REDIS_SET_KEY_HASH_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  } else if (type == StreamValue::TYPE_STREAM) {  // XADD is complex
    StreamValue* stream = static_cast<StreamValue*>(value.get());
    StreamValue::streams_t streams = stream->GetStreams();
    wr << REDIS_SET_KEY_STREAM_COMMAND SPACE_STR << key_str.GetForCommandLine();
    for (size_t i = 0; i < streams.size(); ++i) {
      StreamValue::Stream cur_str = streams[i];
      for (size_t j = 0; j < cur_str.entries.size(); ++j) {
        wr << SPACE_STR << cur_str.sid << SPACE_STR << cur_str.entries[j].name << SPACE_STR << cur_str.entries[j].value;
      }
    }
  } else if (type == JsonValue::TYPE_JSON) {
    wr << REDIS_SET_KEY_JSON_COMMAND SPACE_STR << key_str.GetForCommandLine() << " . " << value.GetForCommandLine();
  } else if (type == GraphValue::TYPE_GRAPH) {
    return NotSupported(GEN_CMD_STRING(REDIS_GRAPH_MODULE_COMMAND("SET")));
  } else if (type == BloomValue::TYPE_BLOOM) {
    return NotSupported(GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("SET")));
  } else if (type == SearchValue::TYPE_FT_INDEX) {
    return NotSupported(GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("INDEX.SET")));
  } else if (type == SearchValue::TYPE_FT_TERM) {
    return NotSupported(GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("TERM.SET")));
  } else {
    wr << REDIS_SET_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::LoadKeyCommandImpl(const NKey& key,
                                                    common::Value::Type type,
                                                    command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  if (type == common::Value::TYPE_ARRAY) {
    wr << REDIS_GET_KEY_ARRAY_COMMAND SPACE_STR << key_str.GetForCommandLine() << " 0 -1";
  } else if (type == JsonValue::TYPE_JSON) {
    wr << REDIS_GET_KEY_JSON_COMMAND SPACE_STR << key_str.GetForCommandLine();
  } else if (type == common::Value::TYPE_SET) {
    wr << REDIS_GET_KEY_SET_COMMAND SPACE_STR << key_str.GetForCommandLine();
  } else if (type == common::Value::TYPE_ZSET) {
    wr << REDIS_GET_KEY_ZSET_COMMAND SPACE_STR << key_str.GetForCommandLine() << " 0 -1 WITHSCORES";
  } else if (type == common::Value::TYPE_HASH) {
    wr << REDIS_GET_KEY_HASH_COMMAND SPACE_STR << key_str.GetForCommandLine();
  } else if (type == StreamValue::TYPE_STREAM) {
    wr << REDIS_GET_KEY_STREAM_COMMAND SPACE_STR << key_str.GetForCommandLine() << " - +";
  } else if (type == GraphValue::TYPE_GRAPH) {
    return NotSupported(GEN_CMD_STRING(REDIS_GRAPH_MODULE_COMMAND("GET")));
  } else if (type == BloomValue::TYPE_BLOOM) {
    return NotSupported(GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("GET")));
  } else if (type == SearchValue::TYPE_FT_INDEX) {
    return NotSupported(GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("INDEX.GET")));
  } else if (type == SearchValue::TYPE_FT_TERM) {
    return NotSupported(GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DOC.GET")));
  } else {
    wr << REDIS_GET_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::DeleteKeyCommandImpl(const NKey& key, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_DELETE_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::RenameKeyCommandImpl(const NKey& key,
                                                      const trans_key_t& new_name,
                                                      command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_RENAME_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << new_name.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::ChangeKeyTTLCommandImpl(const NKey& key,
                                                         ttl_t ttl,
                                                         command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  if (ttl == NO_TTL) {
    wr << REDIS_PERSIST_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  } else {
    wr << REDIS_CHANGE_TTL_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(ttl);
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::LoadKeyTTLCommandImpl(const NKey& key, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_GET_TTL_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

bool CommandTranslator::IsLoadKeyCommandImpl(const CommandInfo& cmd) const {
  return cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_ARRAY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_SET_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_ZSET_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_HASH_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_STREAM_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_JSON_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_INCR_KEY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_DECR_KEY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_INCRBY_KEY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_DECRBY_KEY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(DB_GETUNI_KEY_COMMAND));
}

common::Error CommandTranslator::PublishCommandImpl(const NDbPSChannel& channel,
                                                    const std::string& message,
                                                    command_buffer_t* cmdstring) const {
  std::string channel_str = channel.GetName();
  command_buffer_writer_t wr;
  wr << REDIS_PUBLISH_COMMAND SPACE_STR << channel_str << SPACE_STR << message;
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::SubscribeCommandImpl(const NDbPSChannel& channel, command_buffer_t* cmdstring) const {
  std::string channel_str = channel.GetName();
  command_buffer_writer_t wr;
  wr << REDIS_SUBSCRIBE_COMMAND SPACE_STR << channel_str;
  *cmdstring = wr.str();
  return common::Error();
}

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
