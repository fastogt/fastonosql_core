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

namespace fastonosql {
namespace core {

command_buffer_t StableCommand(const command_buffer_t& command) {
  if (command.empty()) {
    return command_buffer_t();
  }

  std::vector<command_buffer_t> tokens;
  size_t tok = common::Tokenize(command, " ", &tokens);
  command_buffer_t stabled_command;
  if (tok > 0) {
    command_buffer_t part = tokens[0];
    if (part.size() % 4 == 0 && part[0] == '\\' && (part[1] == 'x' || part[1] == 'X')) {
      stabled_command += "\"" + part + "\"";
    } else {
      stabled_command += part;
    }

    for (size_t i = 1; i < tok; ++i) {
      command_buffer_t part = tokens[i];
      stabled_command += " ";
      if (part.size() % 4 == 0 && part[0] == '\\' && (part[1] == 'x' || part[1] == 'X')) {
        stabled_command += "\"" + part + "\"";
      } else {
        stabled_command += part;
      }
    }
  }

  if (stabled_command[stabled_command.size() - 1] == '\r') {
    stabled_command.pop_back();
  }

  return stabled_command;
}

ReadableString::ReadableString() : data_(), type_(TEXT_DATA) {}

ReadableString::ReadableString(const readable_string_t& data) : data_(), type_(TEXT_DATA) {
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
    std::string hexed;
    bool is_ok = common::utils::xhex::encode(data_, is_lower_hex, &hexed);
    DCHECK(is_ok) << "Can't hexed: " << data_;
    return hexed;
  }

  return data_;
}

readable_string_t ReadableString::GetForCommandLine(bool need_quotes) const {
  if (type_ == BINARY_DATA) {
    command_buffer_writer_t wr;
    std::string hexed;
    bool is_ok = common::utils::xhex::encode(data_, is_lower_hex, &hexed);
    DCHECK(is_ok) << "Can't hexed: " << data_;
    wr << "\"" << hexed << "\"";
    return wr.str();
  }

  if (detail::is_json(data_)) {
    return data_;
  }

  if (detail::have_space(data_) && need_quotes) {
    return "\"" + data_ + "\"";
  }

  return data_;
}

void ReadableString::SetData(const readable_string_t& data) {
  data_ = data;
  type_ = detail::is_binary_data(data) ? BINARY_DATA : TEXT_DATA;
}

bool ReadableString::Equals(const ReadableString& other) const {
  return type_ == other.type_ && data_ == other.data_;
}

namespace detail {

bool have_space(const std::string& data) {
  auto it = std::find_if(data.begin(), data.end(), [](char c) { return std::isspace(c); });
  return it != data.end();
}

bool is_binary_data(const command_buffer_t& data) {
  for (size_t i = 0; i < data.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(data[i]);
    if (c < ' ' && c != '\r' && c != '\n') {  // should be hexed symbol
      return true;
    }
  }
  return false;
}

bool is_json(const std::string& data) {
  if (data.empty()) {
    return false;
  }

  return (data[0] == '{' && data[data.size() - 1] == '}') || (data[0] == '[' && data[data.size() - 1] == ']');
}
}  // namespace detail

}  // namespace core
}  // namespace fastonosql
