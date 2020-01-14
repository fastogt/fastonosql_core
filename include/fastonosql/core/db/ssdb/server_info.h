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
#include <vector>

#include <fastonosql/core/server/iserver_info.h>
#include <fastonosql/core/server/state_field.h>

#include <fastonosql/core/db_traits.h>

#define SSDB_COMMON_LABEL "# Common"

#define SSDB_COMMON_VERSION_LABEL "version"
#define SSDB_COMMON_LINKS_LABEL "links"
#define SSDB_COMMON_TOTAL_CALLS_LABEL "total_calls"
#define SSDB_COMMON_DBSIZE_LABEL "dbsize"
#define SSDB_COMMON_BINLOGS_LABEL "binlogs"

namespace fastonosql {
namespace core {
namespace ssdb {

std::vector<common::Value::Type> GetSupportedValueTypes();
std::vector<info_field_t> GetInfoFields();

class ServerInfo : public IServerInfo {
 public:
  struct Stats : IStateField {
    Stats();
    explicit Stats(const std::string& common_text);
    common::Value* GetValueByIndex(unsigned char index) const override;

    std::string version;
    uint32_t links;
    uint32_t total_calls;
    uint32_t dbsize;
    std::string binlogs;
  } stats_;

  ServerInfo();
  explicit ServerInfo(const Stats& common);
  explicit ServerInfo(const std::string& content);

  common::Value* GetValueByIndexes(unsigned char property, unsigned char field) const override;
  std::string ToString() const override;
  uint32_t GetVersion() const override;
};

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
