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
#define REDIS_SET_KEY_SET_COMMAND "SADD"
#define REDIS_SET_KEY_ZSET_COMMAND "ZADD"
#define REDIS_SET_KEY_HASH_COMMAND "HMSET"
#define REDIS_SET_KEY_STREAM_COMMAND "XADD"
#define REDIS_SET_KEY_JSON_COMMAND REDIS_JSON_MODULE_COMMAND("SET")

#define REDIS_GET_KEY_COMMAND DB_GET_KEY_COMMAND
#define REDIS_GET_KEY_ARRAY_COMMAND "LRANGE"
#define REDIS_GET_KEY_SET_COMMAND "SMEMBERS"
#define REDIS_GET_KEY_ZSET_COMMAND "ZRANGE"
#define REDIS_GET_KEY_HASH_COMMAND "HGETALL"
#define REDIS_GET_KEY_STREAM_COMMAND "XRANGE"
#define REDIS_GET_KEY_JSON_COMMAND REDIS_JSON_MODULE_COMMAND("GET")

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

#define REDIS_SETEX "SETEX"
#define REDIS_SETNX "SETNX"

#define REDIS_DECR "DECR"
#define REDIS_DECRBY "DECRBY"

#define REDIS_INCR "INCR"
#define REDIS_INCRBY "INCRBY"
#define REDIS_INCRBYFLOAT "INCRBYFLOAT"

#if defined(PRO_VERSION)
#define REDIS_MODULE_LOAD "MODULE LOAD"
#define REDIS_MODULE_UNLOAD "MODULE UNLOAD"
#endif

namespace fastonosql {
namespace core {
namespace redis_compatible {

CommandTranslator::CommandTranslator(const std::vector<CommandHolder>& commands) : ICommandTranslator(commands) {}

const char* CommandTranslator::GetDBName() const {
  return ConnectionTraits<REDIS>::GetDBName();
}

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

  key_t key_str = key.GetKey();
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
    const auto value_str = value.GetReadableValue();

    wr << SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value_str.GetForCommandLine();
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
    const auto value_str = value.GetReadableValue();

    wr << SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value_str.GetForCommandLine();
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
    key_t key_str = keys[i].GetKey();
    wr << SPACE_STR << key_str.GetForCommandLine();
  }
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Smembers(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_SMEMBERS SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Lrange(const NKey& key, int start, int stop, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_LRANGE SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(start) << SPACE_STR
     << common::ConvertToBytes(stop);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::SetEx(const NDbKValue& key, ttl_t ttl, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();
  const auto value_str = value.GetReadableValue();

  return SetEx(key_str, value_str, ttl, cmdstring);
}

common::Error CommandTranslator::SetEx(const trans_key_t& key,
                                       const trans_key_t& value,
                                       ttl_t ttl,
                                       command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_SETEX SPACE_STR << key.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(ttl) << SPACE_STR
     << value.GetForCommandLine();
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
  const auto value_str = value.GetReadableValue();

  command_buffer_writer_t wr;
  wr << REDIS_SETNX SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Decr(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_DECR SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::DecrBy(const NKey& key, int inc, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_DECRBY SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(inc);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Incr(const NKey& key, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_INCR SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::IncrBy(const NKey& key, int inc, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_INCRBY SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(inc);
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::IncrByFloat(const NKey& key, double inc, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_INCRBYFLOAT SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToBytes(inc);
  *cmdstring = wr.str();
  return common::Error();
}

#if defined(PRO_VERSION)
common::Error CommandTranslator::ModuleLoad(const ModuleInfo& module, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_MODULE_LOAD SPACE_STR << module.name;
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::ModuleUnload(const ModuleInfo& module, command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << REDIS_MODULE_UNLOAD SPACE_STR << module.name;
  *cmdstring = wr.str();
  return common::Error();
}
#endif

common::Error CommandTranslator::PExpire(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const {
  key_t key_str = key.GetKey();
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

common::Error CommandTranslator::CreateKeyCommandImpl(const NDbKValue& key, command_buffer_t* cmdstring) const {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();
  const auto value_str = value.GetReadableValue();

  command_buffer_writer_t wr;
  common::Value::Type type = key.GetType();

  if (type == common::Value::TYPE_ARRAY) {
    wr << REDIS_SET_KEY_ARRAY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR
       << value_str.GetForCommandLine(false);
  } else if (type == common::Value::TYPE_SET) {
    wr << REDIS_SET_KEY_SET_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR
       << value_str.GetForCommandLine(false);
  } else if (type == common::Value::TYPE_ZSET) {
    wr << REDIS_SET_KEY_ZSET_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR
       << value_str.GetForCommandLine(false);
  } else if (type == common::Value::TYPE_HASH) {
    // HMSET gameConfig:1:1 tile "note3" RY "1920" RX "1080" id 1
    wr << REDIS_SET_KEY_HASH_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR
       << value_str.GetForCommandLine(false);
  } else if (type == StreamValue::TYPE_STREAM) {  // XADD is complex
    NValue nv = key.GetValue();
    StreamValue* value = static_cast<StreamValue*>(nv.get());
    StreamValue::streams_t streams = value->GetStreams();
    for (size_t i = 0; i < streams.size(); ++i) {
      StreamValue::Stream cur_str = streams[i];
      wr << REDIS_SET_KEY_STREAM_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << cur_str.sid;
      for (size_t j = 0; j < cur_str.entries.size(); ++j) {
        wr << SPACE_STR << cur_str.entries[j].name << SPACE_STR << cur_str.entries[j].value;
      }
      if (i != streams.size() - 1) {
        wr << END_COMMAND_CHAR;
      }
    }
  } else if (type == JsonValue::TYPE_JSON) {
    wr << REDIS_SET_KEY_JSON_COMMAND SPACE_STR << key_str.GetForCommandLine() << " . " << value_str.GetForCommandLine();
  } else if (type == GraphValue::TYPE_GRAPH) {
    return NotSupported(GEN_CMD_STRING(REDIS_GRAPH_MODULE_COMMAND("SET")));
  } else if (type == BloomValue::TYPE_BLOOM) {
    return NotSupported(GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("SET")));
  } else if (type == SearchValue::TYPE_FT_INDEX) {
    return NotSupported(GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("INDEX.SET")));
  } else if (type == SearchValue::TYPE_FT_TERM) {
    return NotSupported(GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("TERM.SET")));
  } else {
    wr << REDIS_SET_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value_str.GetForCommandLine();
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::LoadKeyCommandImpl(const NKey& key,
                                                    common::Value::Type type,
                                                    command_buffer_t* cmdstring) const {
  key_t key_str = key.GetKey();
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
  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_DELETE_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::RenameKeyCommandImpl(const NKey& key,
                                                      const key_t& new_name,
                                                      command_buffer_t* cmdstring) const {
  key_t key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_RENAME_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << new_name.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::ChangeKeyTTLCommandImpl(const NKey& key,
                                                         ttl_t ttl,
                                                         command_buffer_t* cmdstring) const {
  key_t key_str = key.GetKey();
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
  key_t key_str = key.GetKey();
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
         cmd.IsEqualName(GEN_CMD_STRING(REDIS_GET_KEY_JSON_COMMAND));
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
