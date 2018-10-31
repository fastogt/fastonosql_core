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

#include <common/file_system/file.h>
#include <common/file_system/string_path_utils.h>

#include <fastonosql/core/cdb_connection_client.h>
#include <fastonosql/core/connection_commands_traits.h>
#include <fastonosql/core/internal/command_handler.h>  // for CommandHandler, etc
#include <fastonosql/core/internal/db_connection.h>    // for DBConnection

#include <fastonosql/core/database/idatabase_info.h>

namespace fastonosql {
namespace core {

// for all commands:
// 1) test input
// 2) test connection state
// 3) exec inner api command, return value via CheckResultCommand

// methods naming
// voi CreateDB() => void OnCreatedDB()

template <typename NConnection, typename Config, ConnectionType connection_type>
class CDBConnection : public internal::DBConnection<NConnection, Config, connection_type>,
                      public internal::CommandHandler,
                      public ConnectionCommandsTraits<connection_type> {
 public:
  typedef internal::DBConnection<NConnection, Config, connection_type> db_base_class;
  typedef ConnectionCommandsTraits<connection_type> connection_traits_class;
  typedef readable_string_t raw_key_t;
  typedef readable_string_t raw_value_t;
  typedef readable_string_t pattern_t;
  typedef std::vector<raw_key_t> raw_keys_t;
  typedef std::vector<raw_value_t> raw_values_t;
  typedef std::string db_name_t;
  typedef std::vector<db_name_t> db_names_t;

  CDBConnection(CDBConnectionClient* client, ICommandTranslator* translator)
      : db_base_class(), CommandHandler(translator), client_(client) {}

  virtual db_name_t GetCurrentDBName() const;  //
  common::Error Help(commands_args_t argv,
                     readable_string_t* answer) WARN_UNUSED_RESULT;  //

  common::Error Scan(cursor_t cursor_in,
                     const pattern_t& pattern,
                     keys_limit_t count_keys,
                     raw_keys_t* keys_out,
                     cursor_t* cursor_out) WARN_UNUSED_RESULT;  // nvi
  common::Error Keys(const raw_key_t& key_start,
                     const raw_key_t& key_end,
                     keys_limit_t limit,
                     raw_keys_t* ret) WARN_UNUSED_RESULT;         // nvi
  common::Error DBkcount(keys_limit_t* size) WARN_UNUSED_RESULT;  // nvi
  common::Error FlushDB() WARN_UNUSED_RESULT;                     // nvi
  common::Error Select(const db_name_t& name,
                       IDataBaseInfo** info) WARN_UNUSED_RESULT;         // nvi
  common::Error CreateDB(const db_name_t& name) WARN_UNUSED_RESULT;      // nvi
  common::Error RemoveDB(const db_name_t& name) WARN_UNUSED_RESULT;      // nvi
  common::Error ConfigGetDatabases(db_names_t* dbs) WARN_UNUSED_RESULT;  // nvi
  common::Error Delete(const NKeys& keys,
                       NKeys* deleted_keys) WARN_UNUSED_RESULT;  // nvi
  common::Error Set(const NDbKValue& key,
                    NDbKValue* added_key) WARN_UNUSED_RESULT;  // nvi
  common::Error Get(const NKey& key,
                    NDbKValue* loaded_key) WARN_UNUSED_RESULT;  // nvi
  common::Error Rename(const NKey& key,
                       const key_t& new_key) WARN_UNUSED_RESULT;         // nvi
  common::Error SetTTL(const NKey& key, ttl_t ttl) WARN_UNUSED_RESULT;   // nvi
  common::Error GetTTL(const NKey& key, ttl_t* ttl) WARN_UNUSED_RESULT;  // nvi
  common::Error Quit() WARN_UNUSED_RESULT;                               // nvi
  common::Error JsonDump(cursor_t cursor_in,
                         const pattern_t& pattern,
                         keys_limit_t limit,
                         const common::file_system::ascii_file_string_path& path,
                         cursor_t* cursor_out) WARN_UNUSED_RESULT;  // nvi

 protected:
  common::Error GenerateError(const std::string& cmd, const std::string& descr) WARN_UNUSED_RESULT {
    const std::string buff = common::MemSPrintf("%s function error: %s", cmd, descr);
    return common::make_error(buff);
  }
  CDBConnectionClient* client_;

 private:
  virtual common::Error ScanImpl(cursor_t cursor_in,
                                 const pattern_t& pattern,
                                 keys_limit_t count_keys,
                                 raw_keys_t* keys_out,
                                 cursor_t* cursor_out) = 0;
  virtual common::Error KeysImpl(const raw_key_t& key_start,
                                 const raw_key_t& key_end,
                                 keys_limit_t limit,
                                 raw_keys_t* ret) = 0;
  virtual common::Error DBkcountImpl(keys_limit_t* size) = 0;
  virtual common::Error FlushDBImpl() = 0;

  virtual common::Error SelectImpl(const db_name_t& name, IDataBaseInfo** info) = 0;
  virtual common::Error CreateDBImpl(const db_name_t& name,
                                     IDataBaseInfo** info);  // optional
  virtual common::Error RemoveDBImpl(const db_name_t& name,
                                     IDataBaseInfo** info);  // optional

  virtual common::Error ConfigGetDatabasesImpl(db_names_t* dbs) = 0;
  virtual common::Error DeleteImpl(const NKeys& keys, NKeys* deleted_keys) = 0;
  virtual common::Error SetImpl(const NDbKValue& key, NDbKValue* added_key) = 0;
  virtual common::Error GetImpl(const NKey& key, NDbKValue* loaded_key) = 0;
  virtual common::Error RenameImpl(const NKey& key, const key_t& new_key) = 0;
  virtual common::Error SetTTLImpl(const NKey& key, ttl_t ttl);   // optional
  virtual common::Error GetTTLImpl(const NKey& key, ttl_t* ttl);  // optional
  virtual common::Error QuitImpl() = 0;
  virtual common::Error JsonDumpImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t limit,
                                     const common::file_system::ascii_file_string_path& path,
                                     cursor_t* cursor_out);  // optional;
};

template <typename NConnection, typename Config, ConnectionType ContType>
typename CDBConnection<NConnection, Config, ContType>::db_name_t
CDBConnection<NConnection, Config, ContType>::GetCurrentDBName() const {
  return "default";
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Help(commands_args_t argv, readable_string_t* answer) {
  size_t argc = argv.size();
  if (!answer) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  if (argc == 0) {
    command_buffer_writer_t wr;
    wr << PROJECT_NAME_TITLE << " based on " << connection_traits_class::GetBasedOn() << " "
       << connection_traits_class::GetVersionApi()
       << " \nType: \"help <command>\" for help on <command>\n\"help " ALL_COMMANDS "\" show all supported commands";

    *answer = wr.str();
    return common::Error();
  }

  translator_t tran = GetTranslator();
  if (argc == 1 && argv[0] == GEN_CMD_STRING(ALL_COMMANDS)) {
    std::vector<CommandInfo> cmds = tran->GetCommands();
    command_buffer_writer_t wr;
    for (size_t i = 0; i < cmds.size(); ++i) {
      wr << cmds[i].name;
      if (i != cmds.size() - 1) {
        wr << END_COMMAND_STR;
      }
    }

    *answer = wr.str();
    return common::Error();
  }

  const CommandHolder* cmd = nullptr;
  size_t off = 0;
  common::Error err = tran->FindCommand(argv, &cmd, &off);
  if (err) {
    return err;
  }

  command_buffer_writer_t wr;
  wr << "name: " << cmd->name << "\nsummary: " << cmd->summary << "\nparams: " << cmd->params
     << "\nsince: " << ConvertVersionNumberToReadableString(cmd->since) << "\nexample: " << cmd->example;
  *answer = wr.str();
  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Scan(cursor_t cursor_in,
                                                                 const pattern_t& pattern,
                                                                 keys_limit_t count_keys,
                                                                 raw_keys_t* keys_out,
                                                                 cursor_t* cursor_out) {
  if (!keys_out || !cursor_out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = ScanImpl(cursor_in, pattern, count_keys, keys_out, cursor_out);
  if (err) {
    return err;
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Keys(const raw_key_t& key_start,
                                                                 const raw_key_t& key_end,
                                                                 keys_limit_t limit,
                                                                 raw_keys_t* ret) {
  if (!ret) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = KeysImpl(key_start, key_end, limit, ret);
  if (err) {
    return err;
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::DBkcount(keys_limit_t* size) {
  if (!size) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = DBkcountImpl(size);
  if (err) {
    return err;
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::FlushDB() {
  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = FlushDBImpl();
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnFlushedCurrentDB();
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::CreateDB(const db_name_t& name) {
  if (name.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  if (name == GetCurrentDBName()) {
    DNOTREACHED();
    return common::make_error("Sorry we can't create selected database.");
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  IDataBaseInfo* linfo = nullptr;
  err = CreateDBImpl(name, &linfo);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnCreatedDB(linfo);
  }

  delete linfo;
  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::RemoveDB(const db_name_t& name) {
  if (name.empty()) {
    DNOTREACHED() << "Invalid database name.";
    return common::make_error_inval();
  }

  if (name == GetCurrentDBName()) {
    const std::string error_str = "Sorry we can't remove selected database.";
    DNOTREACHED() << error_str;
    return common::make_error(error_str);
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  IDataBaseInfo* linfo = nullptr;
  err = RemoveDBImpl(name, &linfo);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnRemovedDB(linfo);
  }

  delete linfo;
  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::ConfigGetDatabases(db_names_t* dbs) {
  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  db_names_t ldbs;
  err = ConfigGetDatabasesImpl(&ldbs);
  if (err) {
    return err;
  }

  *dbs = ldbs;
  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Select(const db_name_t& name, IDataBaseInfo** info) {
  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  IDataBaseInfo* linfo = nullptr;
  err = SelectImpl(name, &linfo);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnChangedCurrentDB(linfo);
  }

  if (info) {
    *info = linfo;
  } else {
    delete linfo;
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Delete(const NKeys& keys, NKeys* deleted_keys) {
  if (!deleted_keys) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = DeleteImpl(keys, deleted_keys);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnRemovedKeys(*deleted_keys);
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Set(const NDbKValue& key, NDbKValue* added_key) {
  if (!added_key) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = SetImpl(key, added_key);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnAddedKey(*added_key);
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Get(const NKey& key, NDbKValue* loaded_key) {
  if (!loaded_key) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = GetImpl(key, loaded_key);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnLoadedKey(*loaded_key);
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Rename(const NKey& key, const key_t& new_key) {
  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = RenameImpl(key, new_key);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnRenamedKey(key, new_key);
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::SetTTL(const NKey& key, ttl_t ttl) {
  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = SetTTLImpl(key, ttl);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnChangedKeyTTL(key, ttl);
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::GetTTL(const NKey& key, ttl_t* ttl) {
  if (!ttl) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = GetTTLImpl(key, ttl);
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnLoadedKeyTTL(key, *ttl);
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::Quit() {
  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = QuitImpl();
  if (err) {
    return err;
  }

  if (client_) {
    client_->OnQuited();
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::JsonDump(
    cursor_t cursor_in,
    const pattern_t& pattern,
    keys_limit_t limit,
    const common::file_system::ascii_file_string_path& path,
    cursor_t* cursor_out) {
  if (!cursor_out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  std::string dir = path.GetDirectory();
  if (!common::file_system::is_directory_exist(dir)) {
    const std::string error_msg =
        common::MemSPrintf("Please create directory: %s for " DB_JSONDUMP_COMMAND " command.", dir);
    return common::make_error(error_msg);
  }

  common::Error err = CDBConnection<NConnection, Config, ContType>::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = JsonDumpImpl(cursor_in, pattern, limit, path, cursor_out);
  if (err) {
    return err;
  }

  return common::Error();
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::SetTTLImpl(const NKey& key, ttl_t ttl) {
  UNUSED(key);
  UNUSED(ttl);
  const std::string error_msg = common::MemSPrintf(
      "Sorry, but now " PROJECT_NAME_TITLE " for %s not supported TTL commands.", connection_traits_class::GetDBName());
  return common::make_error(error_msg);
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::GetTTLImpl(const NKey& key, ttl_t* ttl) {
  UNUSED(key);
  UNUSED(ttl);
  const std::string error_msg = common::MemSPrintf(
      "Sorry, but now " PROJECT_NAME_TITLE " for %s not supported TTL commands.", connection_traits_class::GetDBName());
  return common::make_error(error_msg);
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::RemoveDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  UNUSED(name);
  UNUSED(info);
  const std::string error_msg =
      common::MemSPrintf("Sorry, but now " PROJECT_NAME_TITLE " for %s not supported " DB_REMOVEDB_COMMAND " commands.",
                         connection_traits_class::GetDBName());
  return common::make_error(error_msg);
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::CreateDBImpl(const db_name_t& name, IDataBaseInfo** info) {
  UNUSED(name);
  UNUSED(info);
  const std::string error_msg =
      common::MemSPrintf("Sorry, but now " PROJECT_NAME_TITLE " for %s not supported " DB_CREATEDB_COMMAND " commands.",
                         connection_traits_class::GetDBName());
  return common::make_error(error_msg);
}

template <typename NConnection, typename Config, ConnectionType ContType>
common::Error CDBConnection<NConnection, Config, ContType>::JsonDumpImpl(
    cursor_t cursor_in,
    const pattern_t& pattern,
    keys_limit_t limit,
    const common::file_system::ascii_file_string_path& path,
    cursor_t* cursor_out) {
  common::file_system::ANSIFile fl;
  common::ErrnoError errn = fl.Open(path, "wb");
  if (errn) {
    return common::make_error_from_errno(errn);
  }

  bool is_wrote = fl.Write("[{\n");
  if (!is_wrote) {
    fl.Close();
    return common::make_error(common::MemSPrintf("Failed to write start of json file: %s.", path.GetPath()));
  }

  raw_keys_t keys;
  common::Error err = Scan(cursor_in, pattern, limit, &keys, cursor_out);
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    const key_t key_str = keys[i];
    const NKey key(key_str);
    NDbKValue loaded_key;
    err = GetImpl(key, &loaded_key);
    if (err) {
      fl.Close();
      return err;
    }

    const NValue value = loaded_key.GetValue();
    const auto value_str = value.GetReadableValue();
    const auto stabled_key = key_str.GetForCommandLine(false);
    const auto stabled_value = value_str.GetForCommandLine(false);
    command_buffer_writer_t wr;
    wr << stabled_key << ":" << stabled_value;
    if (i != keys.size() - 1) {
      wr << ",";
    }
    wr << "\n";
    readable_string_t buff = wr.str();
    is_wrote = fl.Write(buff);

    if (!is_wrote) {
      fl.Close();
      return common::make_error(common::MemSPrintf("Failed to write entry of json file: %s.", path.GetPath()));
    }
  }

  is_wrote = fl.Write("}]\n");
  if (!is_wrote) {
    fl.Close();
    return common::make_error(common::MemSPrintf("Failed to write end of json file: %s.", path.GetPath()));
  }

  fl.Close();
  return common::Error();
}

}  // namespace core
}  // namespace fastonosql
