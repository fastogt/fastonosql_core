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

#include <fastonosql/core/db/redis/command_translator.h>

#include <fastonosql/core/value.h>

#include <fastonosql/core/connection_types.h>

#define REDIS_XADD_COMMAND "XADD"
#define REDIS_UNLINK_COMMAND "UNLINK"

#if defined(PRO_VERSION)
#define REDIS_MODULE_LOAD "MODULE LOAD"
#define REDIS_MODULE_UNLOAD "MODULE UNLOAD"
#endif

namespace fastonosql {
namespace core {
namespace redis {

CommandTranslator::CommandTranslator(const std::vector<CommandHolder>& commands) : base_class(commands) {}

const char* CommandTranslator::GetDBName() const {
  typedef ConnectionTraits<REDIS> connection_traits_class;
  return connection_traits_class::GetDBName();
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

common::Error CommandTranslator::Xadd(const NDbKValue& key, command_buffer_t* cmdstring) {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();

  StreamValue* stream = static_cast<StreamValue*>(value.get());
  StreamValue::streams_t streams = stream->GetStreams();

  command_buffer_writer_t wr;
  wr << REDIS_XADD_COMMAND SPACE_STR << key_str.GetForCommandLine();
  for (size_t i = 0; i < streams.size(); ++i) {
    StreamValue::Stream cur_str = streams[i];
    wr << SPACE_STR << cur_str.sid;
    for (size_t j = 0; j < cur_str.entries.size(); ++j) {
      wr << SPACE_STR << cur_str.entries[j].name << SPACE_STR << cur_str.entries[j].value;
    }
  }

  *cmdstring = wr.str();
  return common::Error();
}

common::Error CommandTranslator::Unlink(const NKey& key, command_buffer_t* cmdstring) {
  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_UNLINK_COMMAND SPACE_STR << key_str.GetForCommandLine();
  *cmdstring = wr.str();
  return common::Error();
}

}  // namespace redis
}  // namespace core
}  // namespace fastonosql
