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

#include <fastonosql/core/db/ssdb/server_info.h>

#include <common/convert2string.h>

#include <fastonosql/core/db_traits.h>
#include <fastonosql/core/value.h>

#define MARKER "\r\n"

namespace fastonosql {
namespace core {
namespace {

const std::vector<Field> kSsdbCommonFields = {Field(SSDB_COMMON_VERSION_LABEL, common::Value::TYPE_STRING),
                                              Field(SSDB_COMMON_LINKS_LABEL, common::Value::TYPE_UINTEGER),
                                              Field(SSDB_COMMON_TOTAL_CALLS_LABEL, common::Value::TYPE_UINTEGER),
                                              Field(SSDB_COMMON_DBSIZE_LABEL, common::Value::TYPE_UINTEGER),
                                              Field(SSDB_COMMON_BINLOGS_LABEL, common::Value::TYPE_STRING)};

}  // namespace

namespace ssdb {

std::vector<common::Value::Type> GetSupportedValueTypes() {
  return {common::Value::TYPE_BOOLEAN, common::Value::TYPE_INTEGER, common::Value::TYPE_UINTEGER,
          common::Value::TYPE_DOUBLE,  common::Value::TYPE_STRING,  common::Value::TYPE_ARRAY,
          common::Value::TYPE_SET,     common::Value::TYPE_ZSET,    common::Value::TYPE_HASH,
          JsonValue::TYPE_JSON};
}

std::vector<info_field_t> GetInfoFields() {
  return {std::make_pair(SSDB_COMMON_LABEL, kSsdbCommonFields)};
}

ServerInfo::Stats::Stats() {}

ServerInfo::Stats::Stats(const std::string& common_text) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = common_text.find(MARKER, start)) != std::string::npos) {
    std::string line = common_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == SSDB_COMMON_VERSION_LABEL) {
      version = value;
    } else if (field == SSDB_COMMON_LINKS_LABEL) {
      uint32_t llinks;
      if (common::ConvertFromString(value, &llinks)) {
        links = llinks;
      }
    } else if (field == SSDB_COMMON_TOTAL_CALLS_LABEL) {
      uint32_t ltotal_calls;
      if (common::ConvertFromString(value, &ltotal_calls)) {
        total_calls = ltotal_calls;
      }
    } else if (field == SSDB_COMMON_DBSIZE_LABEL) {
      uint32_t ldbsize;
      if (common::ConvertFromString(value, &ldbsize)) {
        dbsize = ldbsize;
      }
    } else if (field == SSDB_COMMON_BINLOGS_LABEL) {
      binlogs = value;
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Stats::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::StringValue(version);
    case 1:
      return new common::FundamentalValue(links);
    case 2:
      return new common::FundamentalValue(total_calls);
    case 3:
      return new common::FundamentalValue(dbsize);
    case 4:
      return new common::StringValue(binlogs);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::ServerInfo() : IServerInfo(SSDB) {}

ServerInfo::ServerInfo(const Stats& common) : IServerInfo(SSDB), stats_(common) {}

common::Value* ServerInfo::GetValueByIndexes(unsigned char property, unsigned char field) const {
  switch (property) {
    case 0:
      return stats_.GetValueByIndex(field);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Stats& value) {
  return out << SSDB_COMMON_VERSION_LABEL ":" << value.version << MARKER << SSDB_COMMON_LINKS_LABEL ":" << value.links
             << MARKER << SSDB_COMMON_TOTAL_CALLS_LABEL ":" << value.total_calls << MARKER
             << SSDB_COMMON_DBSIZE_LABEL ":" << value.dbsize << MARKER << SSDB_COMMON_BINLOGS_LABEL ":" << value.binlogs
             << MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo& value) {
  return out << value.ToString();
}

ServerInfo* MakeSsdbServerInfo(const std::string& content) {
  if (content.empty()) {
    return nullptr;
  }

  ServerInfo* result = new ServerInfo;
  static const std::vector<info_field_t> fields = GetInfoFields();
  std::string word;
  DCHECK_EQ(fields.size(), 1);

  for (size_t i = 0; i < content.size(); ++i) {
    word += content[i];
    if (word == fields[0].first) {
      std::string part = content.substr(i + 1);
      result->stats_ = ServerInfo::Stats(part);
      break;
    }
  }

  return result;
}

std::string ServerInfo::ToString() const {
  std::stringstream str;
  str << SSDB_COMMON_LABEL MARKER << stats_;
  return str.str();
}

uint32_t ServerInfo::GetVersion() const {
  return common::ConvertVersionNumberFromString(stats_.version);
}

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
