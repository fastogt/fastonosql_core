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

#include <fastonosql/core/db/rocksdb/server_info.h>

#include <common/convert2string.h>

#include <fastonosql/core/macros.h>

namespace fastonosql {
namespace core {
namespace {

const std::vector<Field> kRocksdbCommonFields = {
    Field(ROCKSDB_STATS_CAMPACTIONS_LEVEL_LABEL, common::Value::TYPE_UINTEGER),
    Field(ROCKSDB_STATS_FILE_SIZE_MB_LABEL, common::Value::TYPE_UINTEGER),
    Field(ROCKSDB_STATS_TIME_SEC_LABEL, common::Value::TYPE_UINTEGER),
    Field(ROCKSDB_STATS_READ_MB_LABEL, common::Value::TYPE_UINTEGER),
    Field(ROCKSDB_STATS_WRITE_MB_LABEL, common::Value::TYPE_UINTEGER)};

}  // namespace

namespace rocksdb {

std::vector<common::Value::Type> GetSupportedValueTypes() {
  return {common::Value::TYPE_BOOLEAN, common::Value::TYPE_INTEGER, common::Value::TYPE_UINTEGER,
          common::Value::TYPE_DOUBLE, common::Value::TYPE_STRING};
}

std::vector<info_field_t> GetInfoFields() {
  return {std::make_pair(ROCKSDB_STATS_LABEL, kRocksdbCommonFields)};
}

ServerInfo::Stats::Stats() : compactions_level(0), file_size_mb(0), time_sec(0), read_mb(0), write_mb(0) {}

ServerInfo::Stats::Stats(const std::string& common_text) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = common_text.find(MARKER_STR, start)) != std::string::npos) {
    std::string line = common_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == ROCKSDB_STATS_CAMPACTIONS_LEVEL_LABEL) {
      uint32_t lcompactions_level;
      if (common::ConvertFromString(value, &lcompactions_level)) {
        compactions_level = lcompactions_level;
      }
    } else if (field == ROCKSDB_STATS_FILE_SIZE_MB_LABEL) {
      uint32_t lfile_size_mb;
      if (common::ConvertFromString(value, &lfile_size_mb)) {
        file_size_mb = lfile_size_mb;
      }
    } else if (field == ROCKSDB_STATS_TIME_SEC_LABEL) {
      uint32_t ltime_sec;
      if (common::ConvertFromString(value, &ltime_sec)) {
        time_sec = ltime_sec;
      }
    } else if (field == ROCKSDB_STATS_READ_MB_LABEL) {
      uint32_t lread_mb;
      if (common::ConvertFromString(value, &lread_mb)) {
        read_mb = lread_mb;
      }
    } else if (field == ROCKSDB_STATS_WRITE_MB_LABEL) {
      uint32_t lwrite_mb;
      if (common::ConvertFromString(value, &lwrite_mb)) {
        write_mb = lwrite_mb;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Stats::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(compactions_level);
    case 1:
      return new common::FundamentalValue(file_size_mb);
    case 2:
      return new common::FundamentalValue(time_sec);
    case 3:
      return new common::FundamentalValue(read_mb);
    case 4:
      return new common::FundamentalValue(write_mb);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::ServerInfo() : IServerInfo(ROCKSDB) {}

ServerInfo::ServerInfo(const Stats& stats) : IServerInfo(ROCKSDB), stats_(stats) {}

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
  return out << ROCKSDB_STATS_CAMPACTIONS_LEVEL_LABEL COLON_STR << value.compactions_level << MARKER_STR
             << ROCKSDB_STATS_FILE_SIZE_MB_LABEL COLON_STR << value.file_size_mb << MARKER_STR
             << ROCKSDB_STATS_TIME_SEC_LABEL COLON_STR << value.time_sec << MARKER_STR
             << ROCKSDB_STATS_READ_MB_LABEL COLON_STR << value.read_mb << MARKER_STR
             << ROCKSDB_STATS_WRITE_MB_LABEL COLON_STR << value.write_mb << MARKER_STR;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo& value) {
  return out << value.ToString();
}

ServerInfo* MakeRocksdbServerInfo(const std::string& content) {
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
  str << ROCKSDB_STATS_LABEL MARKER_STR << stats_;
  return str.str();
}

uint32_t ServerInfo::GetVersion() const {
  return 0;
}

}  // namespace rocksdb
}  // namespace core
}  // namespace fastonosql
