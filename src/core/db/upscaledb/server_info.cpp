/*  Copyright (C) 2014-2017 FastoGT. All right reserved.

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

#include <fastonosql/core/db/upscaledb/server_info.h>

#include <fastonosql/core/macros.h>

namespace fastonosql {
namespace core {
namespace {

const std::vector<Field> kUpscaledbCommonFields = {Field(UPSCALEDB_FILE_NAME_LABEL, common::Value::TYPE_STRING)};

}  // namespace

namespace upscaledb {

std::vector<common::Value::Type> GetSupportedValueTypes() {
  return {common::Value::TYPE_BOOLEAN, common::Value::TYPE_INTEGER, common::Value::TYPE_UINTEGER,
          common::Value::TYPE_DOUBLE, common::Value::TYPE_STRING};
}

std::vector<info_field_t> GetInfoFields() {
  return {std::make_pair(UPSCALEDB_STATS_LABEL, kUpscaledbCommonFields)};
}

ServerInfo::Stats::Stats() : db_path() {}

ServerInfo::Stats::Stats(const std::string& common_text) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = common_text.find(MARKER_STR, start)) != std::string::npos) {
    std::string line = common_text.substr(start, pos - start);
    size_t delem = line.find_first_of(COLON_CHAR);
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == UPSCALEDB_FILE_NAME_LABEL) {
      db_path = value;
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Stats::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return common::Value::CreateStringValueFromBasicString(db_path);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::ServerInfo() : IServerInfo(UPSCALEDB) {}

ServerInfo::ServerInfo(const Stats& stats) : IServerInfo(UPSCALEDB), stats_(stats) {}

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
  return out << UPSCALEDB_FILE_NAME_LABEL COLON_STR << value.db_path << MARKER_STR;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo& value) {
  return out << value.ToString();
}

ServerInfo* MakeUpscaleDBServerInfo(const std::string& content) {
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
  str << UPSCALEDB_STATS_LABEL MARKER_STR << stats_;
  return str.str();
}

uint32_t ServerInfo::GetVersion() const {
  return 0;
}

}  // namespace upscaledb
}  // namespace core
}  // namespace fastonosql
