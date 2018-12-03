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

#include <fastonosql/core/db/dynomite_redis/command_translator.h>

#include <fastonosql/core/connection_types.h>

namespace fastonosql {
namespace core {
namespace dynomite_redis {

CommandTranslator::CommandTranslator(const std::vector<CommandHolder>& commands) : base_class(commands) {}

const char* CommandTranslator::GetDBName() const {
  typedef ConnectionTraits<DYNOMITE_REDIS> connection_traits_class;
  return connection_traits_class::GetDBName();
}

}  // namespace dynomite_redis
}  // namespace core
}  // namespace fastonosql
