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

#include <limits>

#include <common/macros.h>

#include <fastonosql/core/basic_types.h>

#define NO_TTL -1       // key exists but has no associated expire
#define EXPIRED_TTL -2  // key does not exist

namespace fastonosql {
namespace core {

bool ParseCommandLine(const command_buffer_t& command_line, commands_args_t* out);

namespace detail {
bool is_binary_data(const readable_string_t& data);

bool have_space(const readable_string_t& data);

bool is_json(const readable_string_t& data);
}  // namespace detail

class ReadableString {
 public:
  enum { is_lower_hex = true };
  enum DataType : uint8_t { TEXT_DATA = 0, BINARY_DATA };

  ReadableString();
  ReadableString(const readable_string_t& data);  // not explicit

  DataType GetType() const;

  readable_string_t GetData() const;           // for direct bytes call
  readable_string_t GetHumanReadable() const;  // for diplaying
  readable_string_t GetForCommandLine(
      bool need_quotes = true) const;  // escape if hex, or double quoted if text with space
  void SetData(const readable_string_t& data);

  bool Equals(const ReadableString& other) const;

  static readable_string_t HexData(const readable_string_t& data);

  void Clear();

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

typedef long long ttl_t;  // in seconds or NO_TTL, EXPIRED_TTL
typedef ttl_t pttl_t;
COMPILE_ASSERT(std::numeric_limits<ttl_t>::max() >= NO_TTL && NO_TTL >= std::numeric_limits<ttl_t>::min(),
               "NO_TTL define must be in ttl type range");
COMPILE_ASSERT(std::numeric_limits<ttl_t>::max() >= EXPIRED_TTL && EXPIRED_TTL >= std::numeric_limits<ttl_t>::min(),
               "EXPIRED_TTL define must be in ttl type range");

typedef ReadableString key_t;
typedef ReadableString value_t;

}  // namespace core
}  // namespace fastonosql
