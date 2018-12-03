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

#define ROCKSDB_STATS_LABEL "# Stats"

#define ROCKSDB_STATS_LEVEL_LABEL "level"
#define ROCKSDB_STATS_FILES_LABEL "files"
#define ROCKSDB_STATS_SIZE_LABEL "size"
#define ROCKSDB_STATS_SCORE_LABEL "score"
#define ROCKSDB_STATS_READ_GB_LABEL "read_gb"
#define ROCKSDB_STATS_RN_GB_LABEL "rn_gb"
#define ROCKSDB_STATS_RNP1_GB_LABEL "rnp1_gb"
#define ROCKSDB_STATS_WRITE_GB_LABEL "write_gb"
#define ROCKSDB_STATS_WNEW_GB_LABEL "wnew_gb"
#define ROCKSDB_STATS_MOVED_GB_LABEL "moved_gb"
#define ROCKSDB_STATS_WAMP_LABEL "wamp"
#define ROCKSDB_STATS_RD_MBS_LABEL "rd_mbs"
#define ROCKSDB_STATS_WR_MBS_LABEL "wr_mbs"
#define ROCKSDB_STATS_COMP_SEC_LABEL "comp_sec"
#define ROCKSDB_STATS_COMP_CNT_LABEL "comp_cnt"
#define ROCKSDB_STATS_AVG_SEC_LABEL "avg_sec"
#define ROCKSDB_STATS_KEY_IN_LABEL "key_in"
#define ROCKSDB_STATS_KEY_DROP_LABEL "key_drop"

namespace fastonosql {
namespace core {
namespace rocksdb {

std::vector<common::Value::Type> GetSupportedValueTypes();
std::vector<info_field_t> GetInfoFields();

class ServerInfo : public IServerInfo {
 public:
  // Level    Files   Size     Score Read(GB)  Rn(GB) Rnp1(GB) Write(GB) Wnew(GB) Moved(GB) W-Amp Rd(MB/s) Wr(MB/s)
  // Comp(sec) Comp(cnt) Avg(sec) KeyIn KeyDrop
  struct Stats : IStateField {
    Stats();
    explicit Stats(const std::string& common_text);
    common::Value* GetValueByIndex(unsigned char index) const override;

    std::string level;
    std::string files;
    double size;
    double score;
    double read_gb;
    double rn_gb;
    double rn_p1;
    double write_gb;
    double wnew_gb;
    double moved_gb;
    double wamp;
    double rd_mbs;
    double wr_mbs;
    uint32_t comp_sec;
    uint32_t comp_cnt;
    double avg_sec;
    uint32_t key_in;
    uint32_t key_drop;
  } stats_;

  ServerInfo();
  explicit ServerInfo(const Stats& stats);

  common::Value* GetValueByIndexes(unsigned char property, unsigned char field) const override;
  std::string ToString() const override;
  uint32_t GetVersion() const override;
};

ServerInfo* MakeRocksdbServerInfo(const std::string& content);

}  // namespace rocksdb
}  // namespace core
}  // namespace fastonosql
