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

#include <fastonosql/core/db/redis_base/command_translator.h>

#if defined(PRO_VERSION)
#include <common/convert2string.h>
#include <fastonosql/core/value.h>

#include "core/db/redis_compatible/internal/modules.h"

#define REDIS_CL_THROTTLE REDIS_CL_MODULE_COMMAND("THROTTLE")

#define REDIS_XADD_COMMAND "XADD"
#define REDIS_MODULE_LOAD "MODULE LOAD"
#define REDIS_MODULE_UNLOAD "MODULE UNLOAD"
#endif

namespace fastonosql {
namespace core {
namespace redis_base {

CommandTranslator::CommandTranslator(const std::vector<CommandHolder>& commands) : base_class(commands) {}

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

common::Error CommandTranslator::ClThrottle(const NKey& key,
                                            redis_int_t max_burst,
                                            redis_int_t count_per_period,
                                            redis_int_t period,
                                            redis_int_t quantity,
                                            command_buffer_t* cmdstring) {
  if (!cmdstring) {
    return common::make_error_inval();
  }

  const auto key_str = key.GetKey();
  command_buffer_writer_t wr;
  wr << REDIS_CL_THROTTLE SPACE_STR << key_str.GetForCommandLine() << SPACE_STR << common::ConvertToString(max_burst)
     << SPACE_STR << common::ConvertToString(count_per_period) << SPACE_STR << common::ConvertToString(period);
  if (quantity) {
    wr << SPACE_STR << common::ConvertToString(quantity);
  }
  *cmdstring = wr.str();
  return common::Error();
}

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
#endif

}  // namespace redis_base
}  // namespace core
}  // namespace fastonosql
