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

#include <deque>
#include <string>

#include <common/byte_writer.h>
#include <common/types.h>

#define GEN_CMD_STRING_SIZE(STR, SIZE) common::ByteArray<char>(STR, STR + SIZE)
#define GEN_CMD_STRING(STR) GEN_CMD_STRING_SIZE(STR, sizeof(STR) - 1)

#define GEN_READABLE_STRING_SIZE(STR, SIZE) GEN_CMD_STRING_SIZE(STR, SIZE)
#define GEN_READABLE_STRING(STR) GEN_CMD_STRING(STR)

namespace fastonosql {
namespace core {

typedef char command_buffer_char_t;
typedef common::ByteArray<command_buffer_char_t> command_buffer_t;
typedef common::ByteWriter<command_buffer_char_t, 512> command_buffer_writer_t;
typedef std::deque<command_buffer_t> commands_args_t;
typedef command_buffer_t readable_string_t;

typedef readable_string_t raw_key_t;
typedef readable_string_t raw_value_t;
typedef readable_string_t pattern_t;
typedef std::vector<raw_key_t> raw_keys_t;
typedef std::vector<raw_value_t> raw_values_t;

typedef readable_string_t db_name_t;
typedef std::vector<db_name_t> db_names_t;

typedef uint32_t keys_limit_t;  // UIntegerValue
typedef keys_limit_t cursor_t;

}  // namespace core
}  // namespace fastonosql
