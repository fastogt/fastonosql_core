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

#define DEFAULT_DELIMITER " "
#define ALL_COMMANDS "*"
#define ALL_KEYS_PATTERNS "*"
#define ALL_PUBSUB_CHANNELS "*"
#define NO_KEYS_LIMIT INT32_MAX
#define OK_RESULT "OK"

namespace fastonosql {
namespace core {

typedef char command_buffer_char_t;
typedef std::basic_string<command_buffer_char_t> command_buffer_t;
typedef std::basic_stringstream<command_buffer_char_t> command_buffer_writer_t;
typedef std::deque<command_buffer_t> commands_args_t;
typedef command_buffer_t readable_string_t;
typedef uint32_t keys_limit_t;  // UIntegerValue
typedef keys_limit_t cursor_t;

command_buffer_t StableCommand(const command_buffer_t& command);

namespace detail {
bool is_binary_data(const command_buffer_t& data);

bool have_space(const std::string& data);

bool is_json(const std::string& data);
}  // namespace detail

class ReadableString {
 public:
  static const bool is_lower_hex = true;
  enum DataType { TEXT_DATA = 0, BINARY_DATA };

  ReadableString();
  ReadableString(const readable_string_t& data);  // not explicit

  DataType GetType() const;

  readable_string_t GetData() const;           // for direct bytes call
  readable_string_t GetHumanReadable() const;  // for diplaying
  readable_string_t GetForCommandLine(
      bool need_quotes = true) const;  // escape if hex, or double quoted if text with space
  void SetData(const readable_string_t& data);

  bool Equals(const ReadableString& other) const;

 private:
  readable_string_t data_;
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
