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

#include "core/db/ssdb/internal/commands_api.h"

#include <fastonosql/core/db/ssdb/db_connection.h>  // for DBConnection

namespace fastonosql {
namespace core {
namespace ssdb {

common::Error CommandsApi::Info(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  ServerInfo::Stats statsout;
  common::Error err = ssdb->Info(argv.size() == 1 ? argv[0] : command_buffer_t(), &statsout);
  if (err) {
    return err;
  }

  ServerInfo sinf(statsout);
  common::StringValue* val = common::Value::CreateStringValueFromBasicString(sinf.ToString());
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::ScanSSDB(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t limit;
  if (!common::ConvertFromBytes(argv[2], &limit)) {
    return common::make_error_inval();
  }

  std::vector<DBConnection::raw_key_t> keysout;
  const DBConnection::raw_key_t key_start = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key_end = common::ConvertToString(argv[1]);
  common::Error err = ssdb->ScanSsdb(key_start, key_end, limit, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::DBsize(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  UNUSED(argv);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t dbsize = 0;
  common::Error err = ssdb->DBsize(&dbsize);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(dbsize);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Auth(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  const std::string pass = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Auth(pass);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Setx(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  ttl_t ttl;
  if (!common::ConvertFromBytes(argv[2], &ttl)) {
    return common::make_error_inval();
  }

  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  const DBConnection::raw_value_t value = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Setx(key, value, ttl);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Incr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t incrby;
  if (!common::ConvertFromBytes(argv[1], &incrby)) {
    return common::make_error_inval();
  }

  int64_t ret = 0;
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Incr(key, incrby, &ret);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Rscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t limit;
  if (!common::ConvertFromBytes(argv[2], &limit)) {
    return common::make_error_inval();
  }

  std::vector<std::string> keysout;
  common::Error err = ssdb->Rscan(common::ConvertToString(argv[0]), common::ConvertToString(argv[1]), limit, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<DBConnection::raw_key_t> keysget;
  for (size_t i = 0; i < argv.size(); ++i) {
    keysget.push_back(common::ConvertToString(argv[i]));
  }

  std::vector<DBConnection::raw_key_t> keysout;
  common::Error err = ssdb->MultiGet(keysget, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::map<std::string, std::string> keysset;
  for (size_t i = 0; i < argv.size(); i += 2) {
    keysset[common::ConvertToString(argv[i])] = common::ConvertToString(argv[i + 1]);
  }

  common::Error err = ssdb->MultiSet(keysset);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiDel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (size_t i = 0; i < argv.size(); ++i) {
    keysget.push_back(common::ConvertToString(argv[i]));
  }

  common::Error err = ssdb->MultiDel(keysget);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  DBConnection::raw_value_t ret;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Hget(hkey, key, &ret);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValueFromBasicString(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hgetall(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Hgetall(hkey, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  const DBConnection::raw_value_t value = common::ConvertToString(argv[2]);
  common::Error err = ssdb->Hset(hkey, key, value);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Hdel(hkey, key);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hincr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t incrby;
  if (!common::ConvertFromBytes(argv[2], &incrby)) {
    return common::make_error_inval();
  }

  int64_t res = 0;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Hincr(hkey, key, incrby, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hsize(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Hsize(key, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hclear(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Hclear(key, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hkeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t limit;
  if (!common::ConvertFromBytes(argv[3], &limit)) {
    return common::make_error_inval();
  }

  std::vector<std::string> keysout;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  const DBConnection::raw_key_t key2 = common::ConvertToString(argv[2]);
  common::Error err = ssdb->Hkeys(hkey, key, key2, limit, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t limit;
  if (!common::ConvertFromBytes(argv[3], &limit)) {
    return common::make_error_inval();
  }

  std::vector<std::string> keysout;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  const DBConnection::raw_key_t key2 = common::ConvertToString(argv[2]);
  common::Error err = ssdb->Hscan(hkey, key, key2, limit, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Hrscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t limit;
  if (!common::ConvertFromBytes(argv[3], &limit)) {
    return common::make_error_inval();
  }

  std::vector<std::string> keysout;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  const DBConnection::raw_key_t key2 = common::ConvertToString(argv[2]);
  common::Error err = ssdb->Hrscan(hkey, key, key2, limit, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiHget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (size_t i = 1; i < argv.size(); ++i) {
    const DBConnection::raw_key_t key = common::ConvertToString(argv[i]);
    keysget.push_back(key);
  }

  std::vector<std::string> keysout;
  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->MultiHget(hkey, keysget, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiHset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::map<DBConnection::raw_key_t, DBConnection::raw_value_t> keys;
  for (size_t i = 1; i < argv.size(); i += 2) {
    const DBConnection::raw_key_t key = common::ConvertToString(argv[i]);
    const DBConnection::raw_key_t value = common::ConvertToString(argv[i + 1]);
    keys[key] = value;
  }

  const DBConnection::raw_key_t hkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->MultiHset(hkey, keys);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t ret;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zget(zkey, key, &ret);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t score;
  if (!common::ConvertFromBytes(argv[2], &score)) {
    return common::make_error_inval();
  }

  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zset(zkey, key, score);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zdel(zkey, key);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zincr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t incrby;
  if (!common::ConvertFromBytes(argv[2], &incrby)) {
    return common::make_error_inval();
  }

  int64_t ret = 0;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zincr(zkey, key, incrby, &ret);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zsize(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Zsize(zkey, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zclear(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Zclear(zkey, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zrank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zrank(zkey, key, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zrrank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zrrank(zkey, key, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t offset;
  if (!common::ConvertFromBytes(argv[1], &offset)) {
    return common::make_error_inval();
  }

  uint64_t limit;
  if (!common::ConvertFromBytes(argv[2], &limit)) {
    return common::make_error_inval();
  }

  std::vector<std::string> res;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Zrange(zkey, offset, limit, &res);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(res);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zrrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  uint64_t offset;
  if (!common::ConvertFromBytes(argv[1], &offset)) {
    return common::make_error_inval();
  }

  uint64_t limit;
  if (!common::ConvertFromBytes(argv[2], &limit)) {
    return common::make_error_inval();
  }

  std::vector<std::string> res;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Zrrange(zkey, offset, limit, &res);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(res);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zkeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  int64_t st;
  if (!common::ConvertFromBytes(argv[2], &st)) {
    return common::make_error_inval();
  }

  int64_t end;
  if (!common::ConvertFromBytes(argv[3], &end)) {
    return common::make_error_inval();
  }

  uint64_t limit;
  if (!common::ConvertFromBytes(argv[4], &limit)) {
    return common::make_error_inval();
  }

  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zkeys(zkey, key, &st, &end, limit, &res);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(res);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  int64_t st;
  if (!common::ConvertFromBytes(argv[2], &st)) {
    return common::make_error_inval();
  }

  int64_t end;
  if (!common::ConvertFromBytes(argv[3], &end)) {
    return common::make_error_inval();
  }

  uint64_t limit;
  if (!common::ConvertFromBytes(argv[4], &limit)) {
    return common::make_error_inval();
  }

  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zscan(zkey, key, &st, &end, limit, &res);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(res);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Zrscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  int64_t st;
  if (!common::ConvertFromBytes(argv[2], &st)) {
    return common::make_error_inval();
  }

  int64_t end;
  if (!common::ConvertFromBytes(argv[3], &end)) {
    return common::make_error_inval();
  }

  uint64_t limit;
  if (!common::ConvertFromBytes(argv[4], &limit)) {
    return common::make_error_inval();
  }

  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Zrscan(zkey, key, &st, &end, limit, &res);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(res);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiZget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (size_t i = 1; i < argv.size(); ++i) {
    const DBConnection::raw_key_t key = common::ConvertToString(argv[i]);
    keysget.push_back(key);
  }

  std::vector<std::string> res;
  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->MultiZget(zkey, keysget, &res);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(res);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiZset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::map<std::string, int64_t> keysget;
  for (size_t i = 1; i < argv.size(); i += 2) {
    int64_t val;
    if (!common::ConvertFromBytes(argv[i + 1], &val)) {
      const DBConnection::raw_key_t key = common::ConvertToString(argv[i]);
      keysget[key] = val;
    }
  }

  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->MultiZset(zkey, keysget);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MultiZdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (size_t i = 1; i < argv.size(); ++i) {
    const DBConnection::raw_key_t key = common::ConvertToString(argv[i]);
    keysget.push_back(key);
  }

  const DBConnection::raw_key_t zkey = common::ConvertToString(argv[0]);
  common::Error err = ssdb->MultiZdel(zkey, keysget);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Qpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  const DBConnection::raw_value_t value = common::ConvertToString(argv[1]);
  common::Error err = ssdb->Qpush(key, value);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Qpop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::string ret;
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Qpop(key, &ret);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValueFromBasicString(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Qslice(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t begin;
  if (!common::ConvertFromBytes(argv[1], &begin)) {
    return common::make_error_inval();
  }

  int64_t end;
  if (!common::ConvertFromBytes(argv[2], &end)) {
    return common::make_error_inval();
  }

  std::vector<std::string> keysout;
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Qslice(key, begin, end, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  ar->AppendBasicStrings(keysout);
  FastoObject* child = new FastoObject(out, ar, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Qclear(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  const DBConnection::raw_key_t key = common::ConvertToString(argv[0]);
  common::Error err = ssdb->Qclear(key, &res);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
