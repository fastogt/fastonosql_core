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

#include <fastonosql/core/cdb_connection.h>
#include <fastonosql/core/global.h>

namespace fastonosql {
namespace core {
namespace internal {

// static functions, because linked to commands in static application time
template <class CDBConnection>
struct ApiTraits {
  typedef CDBConnection cdb_connection_t;

  static common::Error Help(CommandHandler* handler, commands_args_t argv, FastoObject* out);  // string

  static common::Error Scan(CommandHandler* handler, commands_args_t argv, FastoObject* out);      // complex array
  static common::Error Keys(CommandHandler* handler, commands_args_t argv, FastoObject* out);      // array of keys
  static common::Error DBkcount(CommandHandler* handler, commands_args_t argv, FastoObject* out);  // keys_limit_t
  static common::Error CreateDatabase(CommandHandler* handler,
                                      commands_args_t argv,
                                      FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error RemoveDatabase(CommandHandler* handler,
                                      commands_args_t argv,
                                      FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error FlushDB(CommandHandler* handler,
                               commands_args_t argv,
                               FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error Select(CommandHandler* handler,
                              commands_args_t argv,
                              FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error Set(CommandHandler* handler,
                           commands_args_t argv,
                           FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error Get(CommandHandler* handler, commands_args_t argv, FastoObject* out);  // common::Value
  static common::Error Rename(CommandHandler* handler,
                              commands_args_t argv,
                              FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error Delete(CommandHandler* handler, commands_args_t argv, FastoObject* out);  // keys_limit_t
  static common::Error SetTTL(CommandHandler* handler,
                              commands_args_t argv,
                              FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error GetTTL(CommandHandler* handler, commands_args_t argv, FastoObject* out);  // ttl_t
  static common::Error Quit(CommandHandler* handler,
                            commands_args_t argv,
                            FastoObject* out);  // GEN_CMD_STRING(OK_RESULT)
  static common::Error ConfigGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);  // array
  static common::Error JsonDump(CommandHandler* handler, commands_args_t argv, FastoObject* out);  // cursor_t
};

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Help(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  command_buffer_t answer;
  common::Error err = cdb->Help(argv, &answer);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(answer);
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Scan(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  cursor_t cursor_in;
  if (!common::ConvertFromBytes(argv[0], &cursor_in)) {
    return common::make_error_inval();
  }

  const size_t argc = argv.size();
  command_buffer_t pattern = argc >= 3 ? argv[2] : GEN_CMD_STRING(ALL_KEYS_PATTERNS);
  cursor_t count_keys = NO_KEYS_LIMIT;
  if (argc >= 5 && !common::ConvertFromBytes(argv[4], &count_keys)) {
    return common::make_error_inval();
  }

  cursor_t cursor_out = 0;
  typename CDBConnection::raw_keys_t keys_out;
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);

  common::Error err = cdb->Scan(cursor_in, pattern, count_keys, &keys_out, &cursor_out);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  for (size_t i = 0; i < keys_out.size(); ++i) {
    common::StringValue* val = common::Value::CreateStringValue(keys_out[i]);
    ar->Append(val);
  }

  common::ArrayValue* mar = common::Value::CreateArrayValue();
  common::FundamentalValue* val = common::Value::CreateUIntegerValue(cursor_out);
  mar->Append(val);
  mar->Append(ar);

  FastoObject* child = new FastoObject(out, mar, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Keys(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);

  keys_limit_t limit;
  if (!common::ConvertFromBytes(argv[2], &limit)) {
    return common::make_error_inval();
  }

  typename CDBConnection::raw_keys_t keysout;
  common::Error err = cdb->Keys(argv[0], argv[1], limit, &keysout);
  if (err) {
    return err;
  }

  common::ArrayValue* ar = common::Value::CreateArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::CreateStringValue(keysout[i]);
    ar->Append(val);
  }
  FastoObject* child = new FastoObject(out, ar, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::DBkcount(internal::CommandHandler* handler,
                                                 commands_args_t argv,
                                                 FastoObject* out) {
  UNUSED(argv);

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);

  keys_limit_t dbkcount = 0;
  common::Error err = cdb->DBkcount(&dbkcount);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(dbkcount);
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::CreateDatabase(CommandHandler* handler,
                                                       commands_args_t argv,
                                                       FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  typename CDBConnection::db_name_t db_name = common::ConvertToString(argv[0]);
  common::Error err = cdb->CreateDB(db_name);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::RemoveDatabase(CommandHandler* handler,
                                                       commands_args_t argv,
                                                       FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  typename CDBConnection::db_name_t db_name = common::ConvertToString(argv[0]);
  common::Error err = cdb->RemoveDB(db_name);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::FlushDB(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  UNUSED(argv);

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  common::Error err = cdb->FlushDB();
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Select(CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  typename CDBConnection::db_name_t db_name = common::ConvertToString(argv[0]);
  common::Error err = cdb->Select(db_name, nullptr);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Set(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  NKey key(argv[0]);

  const common::Value::string_t val_str = argv[1];
  NValue string_val(common::Value::CreateStringValue(val_str));
  NDbKValue kv(key, string_val);

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  NDbKValue key_added;
  common::Error err = cdb->Set(kv, &key_added);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Get(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  NKey key(argv[0]);

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = cdb->Get(key, &key_loaded);
  if (err) {
    return err;
  }

  NValue val = key_loaded.GetValue();
  common::Value* copy = val->DeepCopy();
  FastoObject* child = new FastoObject(out, copy, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Delete(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  NKeys keysdel;
  for (size_t i = 0; i < argv.size(); ++i) {
    NKey key(argv[i]);
    keysdel.push_back(key);
  }

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  NKeys keys_deleted;
  common::Error err = cdb->Delete(keysdel, &keys_deleted);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(keys_deleted.size());  // keys_limit_t
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Rename(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  NKey key(argv[0]);

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  common::Error err = cdb->Rename(key, argv[1]);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::SetTTL(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  NKey key(argv[0]);

  ttl_t ttl;
  if (!common::ConvertFromBytes(argv[1], &ttl)) {
    return common::make_error_inval();
  }

  common::Error err = cdb->SetTTL(key, ttl);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::GetTTL(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  NKey key(argv[0]);

  ttl_t ttl;
  common::Error err = cdb->GetTTL(key, &ttl);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(ttl);
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::Quit(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  UNUSED(argv);

  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  common::Error err = cdb->Quit();
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::ConfigGet(internal::CommandHandler* handler,
                                                  commands_args_t argv,
                                                  FastoObject* out) {
  if (!common::EqualsASCII(argv[0], GEN_CMD_STRING("databases"), false)) {
    return common::make_error_inval();
  }

  std::vector<typename CDBConnection::db_name_t> dbs;
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  common::Error err = cdb->ConfigGetDatabases(&dbs);
  if (err) {
    return err;
  }

  common::ArrayValue* arr = new common::ArrayValue;
  arr->AppendBasicStrings(dbs);
  FastoObject* child = new FastoObject(out, arr, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

template <class CDBConnection>
common::Error ApiTraits<CDBConnection>::JsonDump(CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  cursor_t cursor_in;
  const size_t argc = argv.size();
  if (argc < 3 || !common::ConvertFromBytes(argv[0], &cursor_in)) {
    return common::make_error_inval();
  }

  const std::string json_path = common::ConvertToString(argv[2]);
  if (common::file_system::is_relative_path(json_path)) {
    return common::make_error("Please use absolute path!");
  }

  common::file_system::ascii_file_string_path path(json_path);

  command_buffer_t pattern = argc >= 5 ? argv[4] : GEN_CMD_STRING(ALL_KEYS_PATTERNS);
  cursor_t count_keys = NO_KEYS_LIMIT;
  if (argc == 7 && !common::ConvertFromBytes(argv[6], &count_keys)) {
    return common::make_error_inval();
  }

  cursor_t cursor_out = 0;
  CDBConnection* cdb = static_cast<CDBConnection*>(handler);
  common::Error err = cdb->JsonDump(cursor_in, pattern, count_keys, path, &cursor_out);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(cursor_out);
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace internal
}  // namespace core
}  // namespace fastonosql
