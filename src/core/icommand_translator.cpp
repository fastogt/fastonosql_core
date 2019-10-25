/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

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

#include <fastonosql/core/icommand_translator.h>

#include <common/convert2string.h>
#include <common/sprintf.h>

namespace fastonosql {
namespace core {

command_buffer_t GetKeysPattern(cursor_t cursor_in, const pattern_t& pattern, keys_limit_t count_keys) {
  command_buffer_writer_t wr;
  wr << DB_SCAN_COMMAND SPACE_STR << common::ConvertToBytes(cursor_in) << " MATCH " << pattern << " COUNT "
     << common::ConvertToBytes(count_keys);
  return wr.str();
}

ICommandTranslator::ICommandTranslator(const std::vector<CommandHolder>& commands) : commands_(commands) {}

ICommandTranslator::~ICommandTranslator() {}

common::Error ICommandTranslator::GetDatabasesCommand(command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  *cmdstring = GEN_CMD_STRING(DB_GET_DATABASES_COMMAND);
  return common::Error();
}

common::Error ICommandTranslator::RemoveDBCommand(const db_name_t& name, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << GEN_CMD_STRING(DB_REMOVEDB_COMMAND SPACE_STR) << name;
  *cmdstring = wr.str();
  return common::Error();
}

common::Error ICommandTranslator::CreateDBCommand(const db_name_t& name, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << GEN_CMD_STRING(DB_CREATEDB_COMMAND SPACE_STR) << name;
  *cmdstring = wr.str();
  return common::Error();
}

common::Error ICommandTranslator::SelectDBCommand(const db_name_t& name, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  command_buffer_writer_t wr;
  wr << GEN_CMD_STRING(DB_SELECTDB_COMMAND SPACE_STR) << name;
  *cmdstring = wr.str();
  return common::Error();
}

common::Error ICommandTranslator::FlushDBCommand(command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  *cmdstring = GEN_CMD_STRING(DB_FLUSHDB_COMMAND);
  return common::Error();
}

common::Error ICommandTranslator::DeleteKeyCommand(const NKey& key, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return DeleteKeyCommandImpl(key, cmdstring);
}

common::Error ICommandTranslator::RenameKeyCommand(const NKey& key,
                                                   const trans_key_t& new_name,
                                                   command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return RenameKeyCommandImpl(key, new_name, cmdstring);
}

common::Error ICommandTranslator::CreateKeyCommand(const NDbKValue& key, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return CreateKeyCommandImpl(key, cmdstring);
}

common::Error ICommandTranslator::LoadKeyCommand(const NKey& key,
                                                 common::Value::Type type,
                                                 command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return LoadKeyCommandImpl(key, type, cmdstring);
}

common::Error ICommandTranslator::ChangeKeyTTLCommand(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return ChangeKeyTTLCommandImpl(key, ttl, cmdstring);
}

common::Error ICommandTranslator::LoadKeyTTLCommand(const NKey& key, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return LoadKeyTTLCommandImpl(key, cmdstring);
}

bool ICommandTranslator::IsLoadKeyCommand(const command_buffer_t& cmd, command_buffer_t* key) const {
  if (!key) {
    DNOTREACHED();
    return false;
  }

  commands_args_t standart_argv;
  if (!ParseCommandLine(cmd, &standart_argv)) {
    return false;
  }

  const CommandHolder* cmdh = nullptr;
  size_t off = 0;
  common::Error err = TestCommandLineArgs(standart_argv, &cmdh, &off);
  if (err) {
    return false;
  }

  if (IsLoadKeyCommandImpl(*cmdh)) {
    *key = standart_argv[off];
    return true;
  }

  return false;
}

common::Error ICommandTranslator::GetTypeCommand(const NKey& key, command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << GEN_CMD_STRING(DB_KEY_TYPE_COMMAND SPACE_STR) << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error ICommandTranslator::PublishCommand(const trans_ps_channel_t& channel,
                                                 const std::string& message,
                                                 command_buffer_t* cmdstring) const {
  if (!cmdstring || message.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return PublishCommandImpl(channel, message, cmdstring);
}

common::Error ICommandTranslator::SubscribeCommand(const trans_ps_channel_t& channel,
                                                   command_buffer_t* cmdstring) const {
  if (!cmdstring) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return SubscribeCommandImpl(channel, cmdstring);
}

common::Error ICommandTranslator::InvalidInputArguments(const command_buffer_t& cmd) {
  std::string buff = common::MemSPrintf("Invalid input argument(s) for command: %s.", common::ConvertToString(cmd));
  return common::make_error(buff);
}

common::Error ICommandTranslator::NotSupported(const command_buffer_t& cmd) {
  std::string buff = common::MemSPrintf("Not supported command: %s.", common::ConvertToString(cmd));
  return common::make_error(buff);
}

common::Error ICommandTranslator::UnknownCommand(const command_buffer_t& cmd) {
  std::string buff = common::MemSPrintf("Unknown sequence: '%s'.", common::ConvertToString(cmd));
  return common::make_error(buff);
}

common::Error ICommandTranslator::UnknownSequence(commands_args_t argv) {
  std::string result;
  for (size_t i = 0; i < argv.size(); ++i) {
    result += argv[i].as_string();
    if (i != argv.size() - 1) {
      result += SPACE_STR;
    }
  }
  std::string buff = common::MemSPrintf("Unknown sequence: '%s'.", result);
  return common::make_error(buff);
}

std::vector<CommandInfo> ICommandTranslator::GetCommands() const {
  std::vector<CommandInfo> cmds;
  for (size_t i = 0; i < commands_.size(); ++i) {
    const CommandHolder* cmd = &commands_[i];
    cmds.push_back(*cmd);
  }
  return cmds;
}

common::Error ICommandTranslator::FindCommandFirstName(const command_buffer_t& command_first_name,
                                                       const CommandHolder** info) const {
  if (!info || command_first_name.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  for (size_t i = 0; i < commands_.size(); ++i) {
    const CommandHolder* cmd = &commands_[i];
    if (cmd->IsEqualFirstName(command_first_name)) {
      *info = cmd;
      return common::Error();
    }
  }

  return UnknownCommand(command_first_name);
}

common::Error ICommandTranslator::FindCommand(const command_buffer_t& command,
                                              const CommandHolder** info,
                                              commands_args_t* argv,
                                              size_t* off) const {
  if (command.empty() || !info) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  commands_args_t standart_argv;
  if (!ParseCommandLine(command, &standart_argv)) {
    return common::make_error_inval();
  }

  common::Error err = FindCommand(standart_argv, info, off);
  if (err) {
    return err;
  }

  if (argv) {
    *argv = standart_argv;
  }
  return common::Error();
}

common::Error ICommandTranslator::FindCommand(commands_args_t argv, const CommandHolder** info, size_t* off) const {
  if (!info || argv.empty()) {
    return common::make_error_inval();
  }

  for (size_t i = 0; i < commands_.size(); ++i) {
    const CommandHolder* cmd = &commands_[i];
    size_t loff = 0;
    if (cmd->IsCommand(argv, &loff)) {
      *info = cmd;
      if (off) {
        *off = loff;
      }
      return common::Error();
    }
  }

  return UnknownSequence(argv);
}

common::Error ICommandTranslator::TestCommandArgs(const CommandHolder* cmd, commands_args_t argv) const {
  if (!cmd) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return cmd->TestArgs(argv);
}

common::Error ICommandTranslator::TestCommandLine(const command_buffer_t& cmd) const {
  if (cmd.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  commands_args_t standart_argv;
  if (!ParseCommandLine(cmd, &standart_argv)) {
    return common::make_error_inval();
  }

  const CommandHolder* cmdh = nullptr;
  size_t loff = 0;
  common::Error err = TestCommandLineArgs(standart_argv, &cmdh, &loff);
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error ICommandTranslator::TestCommandLineArgs(commands_args_t argv,
                                                      const CommandHolder** info,
                                                      size_t* off) const {
  const CommandHolder* cmd = nullptr;
  size_t loff = 0;
  common::Error err = FindCommand(argv, &cmd, &loff);
  if (err) {
    return err;
  }

  commands_args_t stabled;
  for (size_t i = loff; i < argv.size(); ++i) {
    stabled.push_back(argv[i]);
  }

  err = TestCommandArgs(cmd, stabled);
  if (err) {
    return err;
  }

  *info = cmd;
  *off = loff;
  return common::Error();
}

}  // namespace core
}  // namespace fastonosql
