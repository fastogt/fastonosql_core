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

#include <fastonosql/core/types.h>

#include <algorithm>

#include <common/string_util.h>

#include <common/convert2string.h>

#include <fastonosql/core/macros.h>

namespace fastonosql {
namespace core {

// CARRIGE_RETURN_CHAR
bool ParseCommandLine(const command_buffer_t& command_line, commands_args_t* out) {
  if (command_line.empty() || !out) {
    return false;
  }

  commands_args_t result;
  command_buffer_t current_string;
  size_t in_single_quotes = 0;
  size_t in_double_quotes = 0;
  size_t in_json = 0;
  size_t in_json_array = 0;

  for (size_t i = 0; i < command_line.size(); ++i) {
    command_buffer_char_t ch = command_line[i];
    const bool is_last = command_line.size() - 1 == i;

    if (current_string.empty() && !in_double_quotes && !in_single_quotes) {
      in_single_quotes = 0;
      in_double_quotes = 0;
      in_json = 0;
      in_json_array = 0;

      if (ch == ' ') {
        for (size_t j = i; j < command_line.size(); ++j) {
          command_buffer_char_t chj = command_line[j];
          if (chj == ' ') {
            i++;
            continue;
          }
          ch = chj;
          break;
        }
      }
    }

    if (in_single_quotes) {
      if (ch == '\\' && !is_last && command_line[i + 1] == SINGLE_QUOTES_CHAR) {
        current_string += command_line[i + 1];
        ++i;
        continue;
      }

      if (ch == '\\') {
        size_t rem = command_line.size() - i;
        if (rem > 4 && (command_line[i + 1] == 'x' || command_line[i + 1] == 'X') &&
            common::IsHexDigit(command_line[i + 2]) && common::IsHexDigit(command_line[i + 3])) {
          char ch = command_line[i + 2];
          char ch2 = command_line[i + 3];
          unsigned char total = common::HexDigitToInt(ch) * 16 + common::HexDigitToInt(ch2);
          current_string.push_back(total);
          i += 3;
          continue;
        }
      }

      // common cases
      if (ch == SINGLE_QUOTES_CHAR) {
        result.push_back(current_string);
        current_string.clear();
        in_single_quotes--;
        continue;
      }
      current_string += ch;
    } else if (in_double_quotes) {
      if (ch == '\\' && !is_last && command_line[i + 1] == DOUBLE_QUOTES_CHAR) {
        current_string += command_line[i + 1];
        ++i;
        continue;
      }

      if (ch == '\\') {
        size_t rem = command_line.size() - i;
        if (rem > 4 && (command_line[i + 1] == 'x' || command_line[i + 1] == 'X') &&
            common::IsHexDigit(command_line[i + 2]) && common::IsHexDigit(command_line[i + 3])) {
          char ch = command_line[i + 2];
          char ch2 = command_line[i + 3];
          unsigned char total = common::HexDigitToInt(ch) * 16 + common::HexDigitToInt(ch2);
          current_string += total;
          i += 3;
          continue;
        }
      }

      // common cases
      if (ch == DOUBLE_QUOTES_CHAR) {
        result.push_back(current_string);
        current_string.clear();
        in_double_quotes--;
        continue;
      }
      current_string += ch;
    } else if (in_json) {
      current_string += ch;
      if (ch == '\\' && !is_last && (command_line[i + 1] == JSON_START_CHAR || command_line[i + 1] == JSON_END_CHAR)) {
        current_string += command_line[i + 1];
        ++i;
        continue;
      }

      // common cases
      if (ch == JSON_START_CHAR) {
        in_json++;
      } else if (ch == JSON_END_CHAR) {
        if (in_json == 1) {
          result.push_back(current_string);
          current_string.clear();
        }
        in_json--;
        continue;
      }
    } else if (in_json_array) {
      current_string += ch;
      if (ch == '\\' && !is_last &&
          (command_line[i + 1] == JSON_START_ARRAY_CHAR || command_line[i + 1] == JSON_END_ARRAY_CHAR)) {
        current_string += command_line[i + 1];
        ++i;
        continue;
      }

      // common cases
      if (ch == JSON_START_ARRAY_CHAR) {
        in_json_array++;
      } else if (ch == JSON_END_ARRAY_CHAR) {
        if (in_json_array == 1) {
          result.push_back(current_string);
          current_string.clear();
        }
        in_json_array--;
        continue;
      }
    } else {
      if (ch == ' ') {
        result.push_back(current_string);
        current_string.clear();
      } else if (ch == SINGLE_QUOTES_CHAR) {
        in_single_quotes = 1;  // skip symbol
      } else if (ch == DOUBLE_QUOTES_CHAR) {
        in_double_quotes = 1;  // skip symbol
      } else if (ch == JSON_START_CHAR) {
        in_json = 1;
        current_string += ch;
      } else if (ch == JSON_START_ARRAY_CHAR) {
        in_json_array = 1;
        current_string += ch;
      } else {
        current_string += ch;
      }
    }
  }

  if (!current_string.empty()) {
    result.push_back(current_string);
  }

  if (in_single_quotes || in_double_quotes || in_json) {
    return false;
  }

  *out = result;
  return true;
}

ReadableString::ReadableString() : data_(), type_(TEXT_DATA) {}

ReadableString::ReadableString(const readable_string_t& data) : data_(), type_(BINARY_DATA) {
  SetData(data);
}

ReadableString::DataType ReadableString::GetType() const {
  return type_;
}

readable_string_t ReadableString::GetData() const {
  return data_;
}

readable_string_t ReadableString::GetHumanReadable() const {
  if (type_ == BINARY_DATA) {
    readable_string_t hexed;
    bool is_ok = common::utils::xhex::encode(data_, is_lower_hex, &hexed);
    DCHECK(is_ok) << "Can't hexed: " << data_;
    return hexed;
  }

  return data_;
}

readable_string_t ReadableString::GetForCommandLine() const {
  if (type_ == BINARY_DATA) {
    return HexData(data_);
  }

  if (detail::is_json(data_)) {
    return data_;
  }

  if (detail::have_space(data_)) {
    command_buffer_writer_t wr;
    wr << SINGLE_QUOTES_CHAR << data_ << SINGLE_QUOTES_CHAR;
    return wr.str();
  }

  return data_;
}

void ReadableString::SetData(const readable_string_t& data) {
  data_ = data;
  type_ = detail::is_binary_data(data_) ? BINARY_DATA : TEXT_DATA;
}

bool ReadableString::Equals(const ReadableString& other) const {
  if (type_ != other.type_) {
    return false;
  }

  return data_ == other.data_;
}

readable_string_t ReadableString::HexData(const readable_string_t& data) {
  readable_string_t hexed;
  bool is_ok = common::utils::xhex::encode(data, is_lower_hex, &hexed);
  DCHECK(is_ok) << "Can't hexed: " << data;
  command_buffer_writer_t wr;
  wr << SINGLE_QUOTES_CHAR << hexed << SINGLE_QUOTES_CHAR;
  return wr.str();
}

void ReadableString::Clear() {
  data_.clear();
}

namespace detail {

bool have_space(const readable_string_t& data) {
  auto it = std::find_if(data.begin(), data.end(), [](readable_string_t::value_type c) { return std::isspace(c); });
  return it != data.end();
}

bool is_binary_data(const readable_string_t& data) {
  for (size_t i = 0; i < data.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(data[i]);
    if (c < SPACE_CHAR && c != CARRIGE_RETURN_CHAR && c != END_LINE_CHAR) {  // should be hexed symbol
      return true;
    }
  }
  return false;
}

bool is_json(const readable_string_t& data) {
  if (data.empty()) {
    return false;
  }

  return (data[0] == JSON_START_CHAR && data[data.size() - 1] == JSON_END_CHAR) ||
         (data[0] == JSON_START_ARRAY_CHAR && data[data.size() - 1] == JSON_END_ARRAY_CHAR);
}

}  // namespace detail

}  // namespace core
}  // namespace fastonosql
