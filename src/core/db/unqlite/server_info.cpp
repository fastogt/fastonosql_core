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

#include <fastonosql/core/db/unqlite/server_info.h>

#include <common/convert2string.h>

#include <fastonosql/core/macros.h>

namespace fastonosql {
namespace core {
namespace unqlite {
namespace {
const std::vector<Field> kUnqliteCommonFields = {
    Field(UNQLITE_STATS_DB_FILE_PATH_LABEL, common::Value::TYPE_STRING),
    Field(UNQLITE_STATS_DB_FILE_SIZE_LABEL, common::Value::TYPE_ULONG_INTEGER)};

std::ostream& operator<<(std::ostream& out, const ServerInfo::Stats& value) {
  return out << UNQLITE_STATS_DB_FILE_PATH_LABEL COLON_STR << value.db_path << MARKER_STR
             << UNQLITE_STATS_DB_FILE_SIZE_LABEL COLON_STR << value.db_size << MARKER_STR;
}
}  // namespace

std::vector<common::Value::Type> GetSupportedValueTypes() {
  return {common::Value::TYPE_STRING};
}

std::vector<info_field_t> GetInfoFields() {
  return {std::make_pair(UNQLITE_STATS_LABEL, kUnqliteCommonFields)};
}

ServerInfo::Stats::Stats() : db_path(), db_size() {}

ServerInfo::Stats::Stats(const std::string& common_text) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = common_text.find(MARKER_STR, start)) != std::string::npos) {
    std::string line = common_text.substr(start, pos - start);
    size_t delem = line.find_first_of(COLON_CHAR);
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == UNQLITE_STATS_DB_FILE_PATH_LABEL) {
      db_path = value;
    } else if (field == UNQLITE_STATS_DB_FILE_SIZE_LABEL) {
      off_t sz;
      if (common::ConvertFromString(value, &sz)) {
        db_size = sz;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Stats::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return common::Value::CreateStringValueFromBasicString(db_path);
    case 1:
      return new common::FundamentalValue(db_size);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::ServerInfo() : IServerInfo() {}

ServerInfo::ServerInfo(const Stats& stats) : IServerInfo(), stats_(stats) {}

ServerInfo::ServerInfo(const std::string& content) {
  static const std::vector<info_field_t> fields = GetInfoFields();
  std::string word;
  DCHECK_EQ(fields.size(), 1);

  for (size_t i = 0; i < content.size(); ++i) {
    word += content[i];
    if (word == fields[0].first) {
      std::string part = content.substr(i + 1);
      stats_ = ServerInfo::Stats(part);
      break;
    }
  }
}

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

std::string ServerInfo::ToString() const {
  std::stringstream str;
  str << UNQLITE_STATS_LABEL MARKER_STR << stats_;
  return str.str();
}

uint32_t ServerInfo::GetVersion() const {
  return 0;
}

}  // namespace unqlite
}  // namespace core
}  // namespace fastonosql
