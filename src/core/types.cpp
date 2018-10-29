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

#include <fastonosql/core/types.h>

#include <algorithm>

#include <common/string_util.h>

#include <common/convert2string.h>

#include <fastonosql/core/macros.h>

namespace fastonosql {
namespace core {

readable_string_t StableCommand(const command_buffer_t& command) {
  if (command.empty()) {
    DNOTREACHED();
    return readable_string_t();
  }

  std::vector<command_buffer_t> tokens;
  size_t tokens_count = common::Tokenize(command, GEN_CMD_STRING(SPACE_STR), &tokens);
  std::stringstream wr;
  if (tokens_count > 0) {
    for (size_t i = 0; i < tokens_count; ++i) {
      command_buffer_t part = tokens[i];
      if (i != 0) {
        wr << SPACE_CHAR;
      }
      if (part.size() % 4 == 0 && part[0] == '\\' && (part[1] == 'x' || part[1] == 'X')) {
        wr << SINGLE_QUOTES_CHAR << part << SINGLE_QUOTES_CHAR;
      } else {
        wr << part;
      }
    }
  }

  readable_string_t stabled_command = wr.str();
  if (!stabled_command.empty()) {
    if (stabled_command[stabled_command.size() - 1] == CARRIGE_RETURN_CHAR) {
      stabled_command.pop_back();
    }
  }

  return stabled_command;
}

ReadableString::ReadableString() : data_(), type_(TEXT_DATA) {}

ReadableString::ReadableString(const readable_string_t& data) : data_(), type_(TEXT_DATA) {
  SetData(data);
}

ReadableString::ReadableString(const readable_string_data_t& data) : data_(), type_(BINARY_DATA) {
  SetData(data);
}

ReadableString::DataType ReadableString::GetType() const {
  return type_;
}

readable_string_data_t ReadableString::GetData() const {
  return data_;
}

readable_string_t ReadableString::GetHumanReadable() const {
  if (type_ == BINARY_DATA) {
    readable_string_data_t hexed;
    bool is_ok = common::utils::xhex::encode(data_, is_lower_hex, &hexed);
    DCHECK(is_ok) << "Can't hexed: " << data_;
    return common::ConvertToString(hexed);
  }

  return common::ConvertToString(data_);
}

readable_string_t ReadableString::GetForCommandLine(bool need_quotes) const {
  if (type_ == BINARY_DATA) {
    return HexData(data_);
  }

  if (detail::is_json(data_)) {
    return common::ConvertToString(data_);
  }

  if (need_quotes && detail::have_space(data_)) {
    return SINGLE_QUOTES_CHAR + common::ConvertToString(data_) + SINGLE_QUOTES_CHAR;
  }

  return common::ConvertToString(data_);
}

void ReadableString::SetData(const readable_string_t& data) {
  data_ = GEN_CMD_STRING_SIZE(data.data(), data.size());
  type_ = detail::is_binary_data(data_) ? BINARY_DATA : TEXT_DATA;
}

void ReadableString::SetData(const readable_string_data_t& data) {
  data_ = data;
  type_ = detail::is_binary_data(data_) ? BINARY_DATA : TEXT_DATA;
}

bool ReadableString::Equals(const ReadableString& other) const {
  return type_ == other.type_ && data_ == other.data_;
}

readable_string_t ReadableString::HexData(const readable_string_data_t& data) {
  readable_string_data_t hexed;
  bool is_ok = common::utils::xhex::encode(data, is_lower_hex, &hexed);
  DCHECK(is_ok) << "Can't hexed: " << data;
  return SINGLE_QUOTES_CHAR + common::ConvertToString(hexed) + SINGLE_QUOTES_CHAR;
  ;
}

namespace detail {

bool have_space(const readable_string_data_t& data) {
  auto it =
      std::find_if(data.begin(), data.end(), [](readable_string_data_t::value_type c) { return std::isspace(c); });
  return it != data.end();
}

bool is_binary_data(const readable_string_data_t& data) {
  for (size_t i = 0; i < data.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(data[i]);
    if (c < SPACE_CHAR && c != CARRIGE_RETURN_CHAR && c != END_LINE_CHAR) {  // should be hexed symbol
      return true;
    }
  }
  return false;
}

bool is_json(const readable_string_data_t& data) {
  if (data.empty()) {
    DNOTREACHED();
    return false;
  }

  return (data[0] == JSON_START_CHAR && data[data.size() - 1] == JSON_END_CHAR) ||
         (data[0] == JSON_START_ARRAY_CHAR && data[data.size() - 1] == JSON_END_ARRAY_CHAR);
}

}  // namespace detail

}  // namespace core
}  // namespace fastonosql
