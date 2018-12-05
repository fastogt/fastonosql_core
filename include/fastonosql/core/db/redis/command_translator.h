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

#include <vector>

#include <fastonosql/core/db/redis_compatible/command_translator.h>

#if defined(PRO_VERSION)
#include <fastonosql/core/module_info.h>
#endif

namespace fastonosql {
namespace core {
namespace redis {

class CommandTranslator : public redis_compatible::CommandTranslator {
 public:
  typedef redis_compatible::CommandTranslator base_class;

  explicit CommandTranslator(const std::vector<CommandHolder>& commands);
  const char* GetDBName() const override;

#if defined(PRO_VERSION)
  common::Error ModuleLoad(const ModuleInfo& module, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error ModuleUnload(const ModuleInfo& module, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
#endif

  common::Error Xadd(const NDbKValue& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error Unlink(const NKey& key, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;

  // zset
  common::Error ZpopMax(const NKey& key, size_t count, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
  common::Error ZpopMin(const NKey& key, size_t count, command_buffer_t* cmdstring) WARN_UNUSED_RESULT;
};

}  // namespace redis
}  // namespace core
}  // namespace fastonosql
