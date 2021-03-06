/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

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

#include <string>

#include <fastonosql/core/types.h>

#define UNDEFINED_ARGS "Undefined"
#define UNDEFINED_SUMMARY "Undefined"
#define UNDEFINED_EXAMPLE_STR "Unspecified"
#define INFINITE_COMMAND_ARGS UINT16_MAX

namespace fastonosql {
namespace core {

struct CommandInfo {
  enum Type : uint8_t { Native = 0, Extended, Internal };
  typedef uint32_t args_size_t;
  CommandInfo(const command_buffer_t& name,
              const std::string& params,
              const std::string& summary,
              uint32_t since,
              const std::string& example,
              args_size_t required_arguments_count,
              args_size_t optional_arguments_count,
              Type type);

  args_size_t GetMaxArgumentsCount() const;
  args_size_t GetMinArgumentsCount() const;

  bool IsEqualName(const command_buffer_t& cmd_name) const;

  const command_buffer_t name;
  const std::string params;
  const std::string summary;
  const uint32_t since;
  const std::string example;

  const args_size_t required_arguments_count;
  const args_size_t optional_arguments_count;
  const Type type;
};

std::string ConvertVersionNumberToReadableString(uint32_t version);

}  // namespace core
}  // namespace fastonosql
