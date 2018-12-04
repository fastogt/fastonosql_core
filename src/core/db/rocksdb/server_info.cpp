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
namespace rocksdb {
namespace {
const std::vector<Field> kRocksdbCommonFields = {Field(ROCKSDB_STATS_LEVEL_LABEL, common::Value::TYPE_STRING),
                                                 Field(ROCKSDB_STATS_FILES_LABEL, common::Value::TYPE_STRING),
                                                 Field(ROCKSDB_STATS_SIZE_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_SCORE_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_READ_GB_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_RN_GB_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_RNP1_GB_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_WRITE_GB_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_WNEW_GB_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_MOVED_GB_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_WAMP_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_RD_MBS_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_WR_MBS_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_COMP_SEC_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_COMP_CNT_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_AVG_SEC_LABEL, common::Value::TYPE_UINTEGER),
                                                 Field(ROCKSDB_STATS_KEY_IN_LABEL, common::Value::TYPE_DOUBLE),
                                                 Field(ROCKSDB_STATS_KEY_DROP_LABEL, common::Value::TYPE_DOUBLE)};

std::ostream& operator<<(std::ostream& out, const ServerInfo::Stats& value) {
  return out << ROCKSDB_STATS_LEVEL_LABEL COLON_STR << value.level << MARKER_STR ROCKSDB_STATS_FILES_LABEL COLON_STR
             << value.files << MARKER_STR ROCKSDB_STATS_SIZE_LABEL COLON_STR << value.size
             << MARKER_STR ROCKSDB_STATS_SCORE_LABEL COLON_STR << value.score
             << MARKER_STR ROCKSDB_STATS_READ_GB_LABEL COLON_STR << value.read_gb
             << MARKER_STR ROCKSDB_STATS_RN_GB_LABEL COLON_STR << value.rn_gb
             << MARKER_STR ROCKSDB_STATS_RNP1_GB_LABEL COLON_STR << value.rn_p1
             << MARKER_STR ROCKSDB_STATS_WRITE_GB_LABEL COLON_STR << value.write_gb
             << MARKER_STR ROCKSDB_STATS_WNEW_GB_LABEL COLON_STR << value.wnew_gb
             << MARKER_STR ROCKSDB_STATS_MOVED_GB_LABEL COLON_STR << value.moved_gb
             << MARKER_STR ROCKSDB_STATS_WAMP_LABEL COLON_STR << value.wamp
             << MARKER_STR ROCKSDB_STATS_RD_MBS_LABEL COLON_STR << value.rd_mbs
             << MARKER_STR ROCKSDB_STATS_WR_MBS_LABEL COLON_STR << value.wr_mbs
             << MARKER_STR ROCKSDB_STATS_COMP_SEC_LABEL COLON_STR << value.comp_sec
             << MARKER_STR ROCKSDB_STATS_COMP_CNT_LABEL COLON_STR << value.comp_cnt
             << MARKER_STR ROCKSDB_STATS_AVG_SEC_LABEL COLON_STR << value.avg_sec
             << MARKER_STR ROCKSDB_STATS_KEY_IN_LABEL COLON_STR << value.key_in
             << MARKER_STR ROCKSDB_STATS_KEY_DROP_LABEL COLON_STR << value.key_drop << MARKER_STR;
}
}  // namespace

std::vector<common::Value::Type> GetSupportedValueTypes() {
  return {common::Value::TYPE_STRING};
}

std::vector<info_field_t> GetInfoFields() {
  return {std::make_pair(ROCKSDB_STATS_LABEL, kRocksdbCommonFields)};
}

ServerInfo::Stats::Stats()
    : level(),
      files(),
      size(0),
      score(0),
      read_gb(0),
      rn_gb(0),
      rn_p1(0),
      write_gb(0),
      wnew_gb(0),
      moved_gb(0),
      wamp(0),
      rd_mbs(0),
      wr_mbs(0),
      comp_sec(0),
      comp_cnt(0),
      avg_sec(0),
      key_in(0),
      key_drop(0) {}

ServerInfo::Stats::Stats(const std::string& common_text) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = common_text.find(MARKER_STR, start)) != std::string::npos) {
    std::string line = common_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == ROCKSDB_STATS_LEVEL_LABEL) {
      level = value;
    } else if (field == ROCKSDB_STATS_FILES_LABEL) {
      files = value;
    } else if (field == ROCKSDB_STATS_SIZE_LABEL) {
      double lsize;
      if (common::ConvertFromString(value, &lsize)) {
        size = lsize;
      }
    } else if (field == ROCKSDB_STATS_SCORE_LABEL) {
      double lscore;
      if (common::ConvertFromString(value, &lscore)) {
        score = lscore;
      }
    } else if (field == ROCKSDB_STATS_READ_GB_LABEL) {
      double lread_gb;
      if (common::ConvertFromString(value, &lread_gb)) {
        read_gb = lread_gb;
      }
    } else if (field == ROCKSDB_STATS_RN_GB_LABEL) {
      double lrn_gb;
      if (common::ConvertFromString(value, &lrn_gb)) {
        rn_gb = lrn_gb;
      }
    } else if (field == ROCKSDB_STATS_RNP1_GB_LABEL) {
      double lrn_p1;
      if (common::ConvertFromString(value, &lrn_p1)) {
        rn_p1 = lrn_p1;
      }
    } else if (field == ROCKSDB_STATS_WRITE_GB_LABEL) {
      double lwrite_gb;
      if (common::ConvertFromString(value, &lwrite_gb)) {
        write_gb = lwrite_gb;
      }
    } else if (field == ROCKSDB_STATS_WNEW_GB_LABEL) {
      double lwnew_gb;
      if (common::ConvertFromString(value, &lwnew_gb)) {
        wnew_gb = lwnew_gb;
      }
    } else if (field == ROCKSDB_STATS_MOVED_GB_LABEL) {
      double lmoved_gb;
      if (common::ConvertFromString(value, &lmoved_gb)) {
        moved_gb = lmoved_gb;
      }
    } else if (field == ROCKSDB_STATS_WAMP_LABEL) {
      double lwamp;
      if (common::ConvertFromString(value, &lwamp)) {
        wamp = lwamp;
      }
    } else if (field == ROCKSDB_STATS_RD_MBS_LABEL) {
      double lrd_mbs;
      if (common::ConvertFromString(value, &lrd_mbs)) {
        rd_mbs = lrd_mbs;
      }
    } else if (field == ROCKSDB_STATS_WR_MBS_LABEL) {
      double lwr_mbs;
      if (common::ConvertFromString(value, &lwr_mbs)) {
        wr_mbs = lwr_mbs;
      }
    } else if (field == ROCKSDB_STATS_COMP_SEC_LABEL) {
      double lcomp_sec;
      if (common::ConvertFromString(value, &lcomp_sec)) {
        comp_sec = lcomp_sec;
      }
    } else if (field == ROCKSDB_STATS_COMP_CNT_LABEL) {
      double lcomp_cnt;
      if (common::ConvertFromString(value, &lcomp_cnt)) {
        comp_cnt = lcomp_cnt;
      }
    } else if (field == ROCKSDB_STATS_AVG_SEC_LABEL) {
      uint32_t lavg_sec;
      if (common::ConvertFromString(value, &lavg_sec)) {
        avg_sec = lavg_sec;
      }
    } else if (field == ROCKSDB_STATS_KEY_IN_LABEL) {
      double lkey_in;
      if (common::ConvertFromString(value, &lkey_in)) {
        key_in = lkey_in;
      }
    } else if (field == ROCKSDB_STATS_KEY_DROP_LABEL) {
      double lkey_drop;
      if (common::ConvertFromString(value, &lkey_drop)) {
        key_drop = lkey_drop;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Stats::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return common::Value::CreateStringValueFromBasicString(level);
    case 1:
      return common::Value::CreateStringValueFromBasicString(files);
    case 2:
      return new common::FundamentalValue(size);
    case 3:
      return new common::FundamentalValue(score);
    case 4:
      return new common::FundamentalValue(read_gb);
    case 5:
      return new common::FundamentalValue(rn_gb);
    case 6:
      return new common::FundamentalValue(rn_p1);
    case 7:
      return new common::FundamentalValue(write_gb);
    case 8:
      return new common::FundamentalValue(wnew_gb);
    case 9:
      return new common::FundamentalValue(moved_gb);
    case 10:
      return new common::FundamentalValue(wamp);
    case 11:
      return new common::FundamentalValue(rd_mbs);
    case 12:
      return new common::FundamentalValue(wr_mbs);
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
  DCHECK_EQ(fields.size(), 1);

  std::string word;
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
  str << ROCKSDB_STATS_LABEL MARKER_STR << stats_;
  return str.str();
}

uint32_t ServerInfo::GetVersion() const {
  return 0;
}

}  // namespace rocksdb
}  // namespace core
}  // namespace fastonosql
