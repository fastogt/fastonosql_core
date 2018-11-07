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

#include "core/db/memcached/internal/commands_api.h"

#include <common/string_util.h>

#include <fastonosql/core/db/memcached/db_connection.h>

namespace fastonosql {
namespace core {
namespace memcached {

common::Error CommandsApi::Info(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* mem = static_cast<DBConnection*>(handler);
  const std::string args = argv.size() == 1 ? common::ConvertToString(argv[0]) : std::string();
  if (!args.empty() && common::FullEqualsASCII(args, "items", false)) {
    commands_args_t largv = {GEN_CMD_STRING("a"), GEN_CMD_STRING("z"), GEN_CMD_STRING("100")};
    return Keys(handler, largv, out);
  }

  ServerInfo::Stats statsout;
  common::Error err = mem->Info(args, &statsout);
  if (err) {
    return err;
  }

  ServerInfo minf(statsout);
  const std::string info_str = minf.ToString();
  common::StringValue* val = common::Value::CreateStringValueFromBasicString(info_str);
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Add(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const auto key_str(argv[0]);
  const NKey key(key_str);
  DBConnection* mem = static_cast<DBConnection*>(handler);
  time_t expiration;
  if (!common::ConvertFromBytes(argv[2], &expiration)) {
    return common::make_error_inval();
  }

  uint32_t flags;
  if (!common::ConvertFromBytes(argv[1], &flags)) {
    return common::make_error_inval();
  }

  common::Error err = mem->AddIfNotExist(key, argv[3], expiration, flags);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Replace(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const auto key_str(argv[0]);
  const NKey key(key_str);
  DBConnection* mem = static_cast<DBConnection*>(handler);
  time_t expiration;
  if (!common::ConvertFromBytes(argv[2], &expiration)) {
    return common::make_error_inval();
  }

  uint32_t flags;
  if (!common::ConvertFromBytes(argv[1], &flags)) {
    return common::make_error_inval();
  }

  common::Error err = mem->Replace(key, argv[3], expiration, flags);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Append(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const auto key_str(argv[0]);
  const NKey key(key_str);
  DBConnection* mem = static_cast<DBConnection*>(handler);
  time_t expiration;
  if (!common::ConvertFromBytes(argv[2], &expiration)) {
    return common::make_error_inval();
  }

  uint32_t flags;
  if (!common::ConvertFromBytes(argv[1], &flags)) {
    return common::make_error_inval();
  }
  common::Error err = mem->Append(key, argv[3], expiration, flags);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Prepend(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const auto key_str(argv[0]);
  const NKey key(key_str);
  DBConnection* mem = static_cast<DBConnection*>(handler);
  time_t expiration;
  if (!common::ConvertFromBytes(argv[2], &expiration)) {
    return common::make_error_inval();
  }

  uint32_t flags;
  if (!common::ConvertFromBytes(argv[1], &flags)) {
    return common::make_error_inval();
  }
  common::Error err = mem->Prepend(key, argv[3], expiration, flags);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Incr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const auto key_str(argv[0]);
  const NKey key(key_str);
  DBConnection* mem = static_cast<DBConnection*>(handler);
  uint32_t value;
  if (!common::ConvertFromBytes(argv[1], &value)) {
    return common::make_error_inval();
  }
  uint64_t result = 0;
  common::Error err = mem->Incr(key, value, &result);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateULongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Decr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const auto key_str(argv[0]);
  const NKey key(key_str);
  DBConnection* mem = static_cast<DBConnection*>(handler);
  uint32_t value;
  if (!common::ConvertFromBytes(argv[1], &value)) {
    return common::make_error_inval();
  }
  uint64_t result = 0;
  common::Error err = mem->Decr(key, value, &result);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateULongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, mem->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace memcached
}  // namespace core
}  // namespace fastonosql
