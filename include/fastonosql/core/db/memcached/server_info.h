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

#define MEMCACHED_COMMON_LABEL "# Common"

#define MEMCACHED_COMMON_PID_LABEL "pid"
#define MEMCACHED_COMMON_UPTIME_LABEL "uptime"
#define MEMCACHED_COMMON_TIME_LABEL "time"
#define MEMCACHED_COMMON_VERSION_LABEL "version"
#define MEMCACHED_COMMON_POINTER_SIZE_LABEL "pointer_size"
#define MEMCACHED_COMMON_RUSAGE_USER_LABEL "rusage_user"
#define MEMCACHED_COMMON_RUSAGE_SYSTEM_LABEL "rusage_system"
#define MEMCACHED_COMMON_CURR_ITEMS_LABEL "curr_items"
#define MEMCACHED_COMMON_TOTAL_ITEMS_LABEL "total_items"
#define MEMCACHED_COMMON_BYTES_LABEL "bytes"
#define MEMCACHED_COMMON_CURR_CONNECTIONS_LABEL "curr_connections"
#define MEMCACHED_COMMON_TOTAL_CONNECTIONS_LABEL "total_connections"
#define MEMCACHED_COMMON_CONNECTION_STRUCTURES_LABEL "connection_structures"
#define MEMCACHED_COMMON_CMD_GET_LABEL "cmd_get"
#define MEMCACHED_COMMON_CMD_SET_LABEL "cmd_set"
#define MEMCACHED_COMMON_GET_HITS_LABEL "get_hits"
#define MEMCACHED_COMMON_GET_MISSES_LABEL "get_misses"
#define MEMCACHED_COMMON_EVICTIONS_LABEL "evictions"
#define MEMCACHED_COMMON_BYTES_READ_LABEL "bytes_read"
#define MEMCACHED_COMMON_BYTES_WRITTEN_LABEL "bytes_written"
#define MEMCACHED_COMMON_LIMIT_MAXBYTES_LABEL "limit_maxbytes"
#define MEMCACHED_COMMON_THREADS_LABEL "threads"

namespace fastonosql {
namespace core {
namespace memcached {

std::vector<common::Value::Type> GetSupportedValueTypes();
std::vector<info_field_t> GetInfoFields();

class ServerInfo : public IServerInfo {
 public:
  struct Stats : IStateField {
    Stats();
    explicit Stats(const std::string& common_text);
    common::Value* GetValueByIndex(unsigned char index) const override;

    uint32_t pid;
    uint32_t uptime;
    uint32_t time;
    std::string version;
    uint32_t pointer_size;
    uint32_t rusage_user;
    uint32_t rusage_system;
    uint32_t curr_items;
    uint32_t total_items;
    uint32_t bytes;
    uint32_t curr_connections;
    uint32_t total_connections;
    uint32_t connection_structures;
    uint32_t cmd_get;
    uint32_t cmd_set;
    uint32_t get_hits;
    uint32_t get_misses;
    uint32_t evictions;
    uint32_t bytes_read;
    uint32_t bytes_written;
    uint32_t limit_maxbytes;
    uint32_t threads;
  } stats_;

  ServerInfo();
  explicit ServerInfo(const Stats& common);

  common::Value* GetValueByIndexes(unsigned char property, unsigned char field) const override;
  std::string ToString() const override;
  uint32_t GetVersion() const override;
};

ServerInfo* MakeMemcachedServerInfo(const std::string& content);

}  // namespace memcached
}  // namespace core
}  // namespace fastonosql
