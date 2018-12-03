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

#include <string>
#include <vector>

#include <fastonosql/core/server/iserver_info.h>
#include <fastonosql/core/server/state_field.h>

#include <fastonosql/core/db_traits.h>

#define UPSCALEDB_STATS_LABEL "# Stats"

#define UPSCALEDB_FILE_NAME_LABEL "db_path"

namespace fastonosql {
namespace core {
namespace upscaledb {

std::vector<common::Value::Type> GetSupportedValueTypes();
std::vector<info_field_t> GetInfoFields();

class ServerInfo : public IServerInfo {
 public:
  // Compactions\nLevel  Files Size(MB) Time(sec) Read(MB)
  // Write(MB)\n
  struct Stats : IStateField {
    Stats();
    explicit Stats(const std::string& common_text);
    common::Value* GetValueByIndex(unsigned char index) const override;

    std::string db_path;
  } stats_;

  ServerInfo();
  explicit ServerInfo(const Stats& stats);

  common::Value* GetValueByIndexes(unsigned char property, unsigned char field) const override;
  std::string ToString() const override;
  uint32_t GetVersion() const override;
};

ServerInfo* MakeUpscaleDBServerInfo(const std::string& content);

}  // namespace upscaledb
}  // namespace core
}  // namespace fastonosql
