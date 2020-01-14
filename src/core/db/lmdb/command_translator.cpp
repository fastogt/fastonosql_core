/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

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

#include <fastonosql/core/db/lmdb/command_translator.h>

#include <fastonosql/core/connection_types.h>

#define LMDB_SET_KEY_COMMAND DB_SET_KEY_COMMAND
#define LMDB_GET_KEY_COMMAND DB_GET_KEY_COMMAND
#define LMDB_DELETE_KEY_COMMAND DB_DELETE_KEY_COMMAND
#define LMDB_RENAME_KEY_COMMAND DB_RENAME_KEY_COMMAND

namespace fastonosql {
namespace core {
namespace lmdb {

CommandTranslator::CommandTranslator(const std::vector<CommandHolder>& commands) : ICommandTranslatorBase(commands) {}

const char* CommandTranslator::GetDBName() const {
  typedef ConnectionTraits<LMDB> connection_traits_class;
  return connection_traits_class::GetDBName();
}

common::Error CommandTranslator::CreateKeyCommandImpl(const NDbKValue& key, command_buffer_t* cmdstring) const {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();

  command_buffer_writer_t wr;
  wr << LMDB_SET_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << value.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::LoadKeyCommandImpl(const NKey& key,
                                                    common::Value::Type type,
                                                    command_buffer_t* cmdstring) const {
  UNUSED(type);

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << LMDB_GET_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::DeleteKeyCommandImpl(const NKey& key, command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << LMDB_DELETE_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::RenameKeyCommandImpl(const NKey& key,
                                                      const nkey_t& new_name,
                                                      command_buffer_t* cmdstring) const {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << LMDB_RENAME_KEY_COMMAND SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << new_name.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

bool CommandTranslator::IsLoadKeyCommandImpl(const CommandInfo& cmd) const {
  return cmd.IsEqualName(GEN_CMD_STRING(LMDB_GET_KEY_COMMAND)) ||
         cmd.IsEqualName(GEN_CMD_STRING(DB_GETUNI_KEY_COMMAND));
}

}  // namespace lmdb
}  // namespace core
}  // namespace fastonosql
