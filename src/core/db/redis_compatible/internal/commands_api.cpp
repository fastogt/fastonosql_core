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

#include "core/db/redis_compatible/internal/commands_api.h"

#include <string>

#include <fastonosql/core/db/redis_compatible/db_connection.h>

#include <fastonosql/core/value.h>

#define WITHSCORES "WITHSCORES"

namespace fastonosql {
namespace core {
namespace redis_compatible {

commands_args_t ExpandCommand(std::initializer_list<command_buffer_t> list, commands_args_t argv) {
  argv.insert(argv.begin(), list);
  return argv;
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Auth(internal::CommandHandler* handler,
                                              commands_args_t argv,
                                              FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  common::Error err = red->Auth(argv[0]);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Info(internal::CommandHandler* handler,
                                              commands_args_t argv,
                                              FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(DB_INFO_COMMAND)}, argv), out);
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Lpush(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::ArrayValue* arr = common::Value::CreateArrayValue();
  for (size_t i = 1; i < argv.size(); ++i) {
    arr->AppendString(argv[i]);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long list_len = 0;
  common::Error err = redis->Lpush(key, NValue(arr), &list_len);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(list_len);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::LFastoSet(internal::CommandHandler* handler,
                                                   commands_args_t argv,
                                                   FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::ArrayValue* arr = common::Value::CreateArrayValue();
  for (size_t i = argv.size() - 1; i > 0; --i) {
    arr->AppendString(argv[i]);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long list_len = 0;
  common::Error err = redis->LFastoSet(key, NValue(arr), &list_len);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(list_len);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Lrange(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  int start;
  if (!common::ConvertFromBytes(argv[1], &start)) {
    return common::make_error_inval();
  }

  int stop;
  if (!common::ConvertFromBytes(argv[2], &stop)) {
    return common::make_error_inval();
  }
  DBConnection* redis = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = redis->Lrange(key, start, stop, &key_loaded);
  if (err) {
    return err;
  }

  NValue val = key_loaded.GetValue();
  common::Value* copy = val->DeepCopy();
  FastoObject* child = new FastoObject(out, copy, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Sadd(internal::CommandHandler* handler,
                                              commands_args_t argv,
                                              FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::SetValue* set = common::Value::CreateSetValue();
  for (size_t i = 1; i < argv.size(); ++i) {
    set->Insert(argv[i]);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long added_items = 0;
  common::Error err = redis->Sadd(key, NValue(set), &added_items);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(added_items);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::SFastoSet(internal::CommandHandler* handler,
                                                   commands_args_t argv,
                                                   FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::SetValue* set = common::Value::CreateSetValue();
  for (size_t i = 1; i < argv.size(); ++i) {
    set->Insert(argv[i]);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long list_len = 0;
  common::Error err = redis->SFastoSet(key, NValue(set), &list_len);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(list_len);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Smembers(internal::CommandHandler* handler,
                                                  commands_args_t argv,
                                                  FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* redis = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = redis->Smembers(key, &key_loaded);
  if (err) {
    return err;
  }

  NValue val = key_loaded.GetValue();
  common::Value* copy = val->DeepCopy();
  FastoObject* child = new FastoObject(out, copy, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Zadd(internal::CommandHandler* handler,
                                              commands_args_t argv,
                                              FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::ZSetValue* zset = common::Value::CreateZSetValue();
  for (size_t i = 1; i < argv.size(); i += 2) {
    command_buffer_t score = argv[i];
    double score_value;
    if (!common::ConvertFromBytes(score, &score_value)) {
      return common::make_error("Score must be floating point number.");
    }
    command_buffer_t member = argv[i + 1];
    zset->Insert(score, member);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long added_items = 0;
  common::Error err = redis->Zadd(key, NValue(zset), &added_items);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(added_items);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::ZFastoSet(internal::CommandHandler* handler,
                                                   commands_args_t argv,
                                                   FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::ZSetValue* zset = common::Value::CreateZSetValue();
  for (size_t i = 1; i < argv.size(); i += 2) {
    command_buffer_t score = argv[i];
    double score_value;
    if (!common::ConvertFromBytes(score, &score_value)) {
      return common::make_error("Score must be floating point number.");
    }
    command_buffer_t member = argv[i + 1];
    zset->Insert(score, member);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long added_items = 0;
  common::Error err = redis->ZFastoSet(key, NValue(zset), &added_items);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(added_items);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Zrange(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  int start;
  if (!common::ConvertFromBytes(argv[1], &start)) {
    return common::make_error_inval();
  }

  int stop;
  if (!common::ConvertFromBytes(argv[2], &stop)) {
    return common::make_error_inval();
  }
  bool ws = argv.size() == 4 && argv[3] == GEN_CMD_STRING(WITHSCORES);
  DBConnection* redis = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = redis->Zrange(key, start, stop, ws, &key_loaded);
  if (err) {
    return err;
  }

  NValue val = key_loaded.GetValue();
  common::Value* copy = val->DeepCopy();
  FastoObject* child = new FastoObject(out, copy, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Hmset(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::HashValue* hmset = common::Value::CreateHashValue();
  for (size_t i = 1; i < argv.size(); i += 2) {
    command_buffer_t key = argv[i];
    command_buffer_t val = argv[i + 1];
    hmset->Insert(key, val);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  common::Error err = redis->Hmset(key, NValue(hmset));
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::HFastoSet(internal::CommandHandler* handler,
                                                   commands_args_t argv,
                                                   FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::HashValue* hmset = common::Value::CreateHashValue();
  for (size_t i = 1; i < argv.size(); i += 2) {
    command_buffer_t key = argv[i];
    command_buffer_t val = argv[i + 1];
    hmset->Insert(key, val);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  common::Error err = redis->HFastoSet(key, NValue(hmset));
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Hgetall(internal::CommandHandler* handler,
                                                 commands_args_t argv,
                                                 FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* redis = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = redis->Hgetall(key, &key_loaded);
  if (err) {
    return err;
  }

  NValue val = key_loaded.GetValue();
  common::Value* copy = val->DeepCopy();
  FastoObject* child = new FastoObject(out, copy, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

// string
template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Append(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);
  const NValue string_val(common::Value::CreateStringValue(argv[1]));

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->Append(key, string_val, &result);
  if (err) {
    return err;
  }

  auto* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::SetEx(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  ttl_t ttl;
  if (!common::ConvertFromBytes(argv[1], &ttl)) {
    return common::make_error_inval();
  }

  const NValue string_val(common::Value::CreateStringValue(argv[2]));
  const NDbKValue kv(key, string_val);

  DBConnection* redis = static_cast<DBConnection*>(handler);
  common::Error err = redis->SetEx(kv, ttl);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::SetNX(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  const NValue string_val(common::Value::CreateStringValue(argv[1]));
  const NDbKValue kv(key, string_val);
  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->SetNX(kv, &result);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Decr(internal::CommandHandler* handler,
                                              commands_args_t argv,
                                              FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->Decr(key, &result);
  if (err) {
    return err;
  }

  auto* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::DecrBy(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  int incr;
  if (!common::ConvertFromBytes(argv[1], &incr)) {
    return common::make_error_inval();
  }
  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->DecrBy(key, incr, &result);
  if (err) {
    return err;
  }

  auto* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Incr(internal::CommandHandler* handler,
                                              commands_args_t argv,
                                              FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->Incr(key, &result);
  if (err) {
    return err;
  }

  auto* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::IncrBy(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  int incr;
  if (!common::ConvertFromBytes(argv[1], &incr)) {
    return common::make_error_inval();
  }
  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->IncrBy(key, incr, &result);
  if (err) {
    return err;
  }

  auto* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::IncrByFloat(internal::CommandHandler* handler,
                                                     commands_args_t argv,
                                                     FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  double incr;
  if (!common::ConvertFromBytes(argv[1], &incr)) {
    return common::make_error_inval();
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  command_buffer_t result;
  common::Error err = redis->IncrByFloat(key, incr, &result);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::Persist(internal::CommandHandler* handler,
                                                 commands_args_t argv,
                                                 FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* red = static_cast<DBConnection*>(handler);
  common::Error err = red->SetTTL(key, NO_TTL);  // PERSIST
  if (err) {
    common::FundamentalValue* val = common::Value::CreateUIntegerValue(0);
    FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
    out->AddChildren(child);
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(1);
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <typename DBConnection>
common::Error CommandsApi<DBConnection>::DBSize(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  UNUSED(argv);

  DBConnection* redis = static_cast<DBConnection*>(handler);

  keys_limit_t dbkcount = 0;
  common::Error err = redis->DBSize(&dbkcount);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(dbkcount);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql

#if defined(BUILD_WITH_REDIS)
#include <fastonosql/core/db/redis/db_connection.h>
#endif
#if defined(BUILD_WITH_PIKA)
#include <fastonosql/core/db/pika/db_connection.h>
#endif
#if defined(BUILD_WITH_DYNOMITE)
#include <fastonosql/core/db/dynomite/db_connection.h>
#endif

namespace fastonosql {
namespace core {
namespace redis_compatible {
#if defined(BUILD_WITH_REDIS)
template class CommandsApi<redis::DBConnection>;
#endif
#if defined(BUILD_WITH_PIKA)
template class CommandsApi<pika::DBConnection>;
#endif
#if defined(BUILD_WITH_DYNOMITE)
template class CommandsApi<dynomite::DBConnection>;
#endif
}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
