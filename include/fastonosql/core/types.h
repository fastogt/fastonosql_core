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

namespace fastonosql {
namespace core {

typedef char command_buffer_char_t;
typedef common::ByteArray<command_buffer_char_t> command_buffer_t;
typedef common::ByteWriter<command_buffer_char_t, 512> command_buffer_writer_t;
typedef std::deque<command_buffer_t> commands_args_t;

typedef char readable_string_char_t;
typedef std::basic_string<readable_string_char_t> readable_string_t;
typedef command_buffer_t readable_string_data_t;

typedef uint32_t keys_limit_t;  // UIntegerValue
typedef keys_limit_t cursor_t;

readable_string_t StableCommand(const command_buffer_t& command);

namespace detail {
bool is_binary_data(const readable_string_data_t& data);

bool have_space(const readable_string_data_t& data);

bool is_json(const readable_string_data_t& data);
}  // namespace detail

class ReadableString {
 public:
  enum { is_lower_hex = true };
  enum DataType : uint8_t { TEXT_DATA = 0, BINARY_DATA };

  ReadableString();
  ReadableString(const readable_string_t& data);       // not explicit
  ReadableString(const readable_string_data_t& data);  // not explicit

  DataType GetType() const;

  readable_string_data_t GetData() const;      // for direct bytes call
  readable_string_t GetHumanReadable() const;  // for diplaying
  readable_string_t GetForCommandLine(
      bool need_quotes = true) const;  // escape if hex, or double quoted if text with space
  void SetData(const readable_string_t& data);
  void SetData(const readable_string_data_t& data);

  bool Equals(const ReadableString& other) const;

  static readable_string_t HexData(const readable_string_data_t& data);

 private:
  readable_string_data_t data_;
  DataType type_;
};

inline bool operator==(const ReadableString& r, const ReadableString& l) {
  return r.Equals(l);
}

inline bool operator!=(const ReadableString& r, const ReadableString& l) {
  return !(r == l);
}

}  // namespace core
}  // namespace fastonosql
