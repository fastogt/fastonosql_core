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

#include "core/internal/commands_api.h"  // for ApiTraits

namespace fastonosql {
namespace core {
namespace redis_compatible {

commands_args_t ExpandCommand(std::initializer_list<command_buffer_t> list, commands_args_t argv);

template <typename DBConnection>
struct CommandsApi : public internal::ApiTraits<DBConnection> {
  // db
  static common::Error Auth(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Info(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // array
  static common::Error Lpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error LfastoSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Lrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // set
  static common::Error Sadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Smembers(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // zset
  static common::Error Zadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // hash
  static common::Error Hmset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hgetall(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // string
  static common::Error Append(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  static common::Error SetEx(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SetNX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  static common::Error Decr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error DecrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  static common::Error Incr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error IncrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error IncrByFloat(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // ttl
  static common::Error Persist(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
};

}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
