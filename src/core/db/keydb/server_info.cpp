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

#include <fastonosql/core/db/keydb/server_info.h>

#include <common/convert2string.h>

#include <fastonosql/core/db_traits.h>
#include <fastonosql/core/value.h>

#include <fastonosql/core/macros.h>
#include <fastonosql/core/types.h>

#define KEYDB_INFO_MARKER MARKER_STR

namespace fastonosql {
namespace core {
namespace keydb {
namespace {

const std::vector<Field> kRedisServerFields = {
    Field(KEYDB_SERVER_VERSION_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_GIT_SHA1_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_GIT_DIRTY_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_BUILD_ID_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_MODE_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_OS_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_ARCH_BITS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_MULTIPLEXING_API_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_GCC_VERSION_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_PROCESS_ID_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_RUN_ID_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_TCP_PORT_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_UPTIME_IN_SECONDS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_UPTIME_IN_DAYS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_HZ_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_LRU_CLOCK_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_SERVER_EXECUTABLE_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_SERVER_CONFIG_FILE_LABEL, common::Value::TYPE_STRING)};

const std::vector<Field> kRedisClientFields = {
    Field(KEYDB_CLIENTS_CONNECTED_CLIENTS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_CLIENTS_CLIENT_LONGEST_OUTPUT_LIST_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_CLIENTS_CLIENT_BIGGEST_INPUT_BUF_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_CLIENTS_BLOCKED_CLIENTS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_CLIENTS_THREAD_0_CLIENTS_LABEL, common::Value::TYPE_UINTEGER)};

const std::vector<Field> kRedisMemoryFields = {
    Field(KEYDB_MEMORY_USED_MEMORY_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_MEMORY_USED_MEMORY_HUMAN_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_MEMORY_USED_MEMORY_RSS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_MEMORY_USED_MEMORY_PEAK_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_MEMORY_USED_MEMORY_PEAK_HUMAN_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_MEMORY_USED_MEMORY_LUA_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_MEMORY_MEM_FRAGMENTATION_RATIO_LABEL, common::Value::TYPE_DOUBLE),
    Field(KEYDB_MEMORY_MEM_ALLOCATOR_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_MEMORY_ACTIVE_DEFRAG_RUNNING_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_MEMORY_LAZYFREE_PENDING_OBJECTS_LABEL, common::Value::TYPE_UINTEGER)};

const std::vector<Field> kRedisPersistenceFields = {
    Field(KEYDB_PERSISTENCE_LOADING_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_RDB_CHANGES_SINCE_LAST_SAVE_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_RDB_DGSAVE_IN_PROGRESS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_RDB_LAST_SAVE_TIME_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_RDB_LAST_DGSAVE_STATUS_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_PERSISTENCE_RDB_LAST_DGSAVE_TIME_SEC_LABEL, common::Value::TYPE_INTEGER),
    Field(KEYDB_PERSISTENCE_RDB_CURRENT_DGSAVE_TIME_SEC_LABEL, common::Value::TYPE_INTEGER),
    Field(KEYDB_PERSISTENCE_AOF_ENABLED_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_AOF_REWRITE_IN_PROGRESS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_AOF_REWRITE_SHEDULED_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_PERSISTENCE_AOF_LAST_REWRITE_TIME_SEC_LABEL, common::Value::TYPE_INTEGER),
    Field(KEYDB_PERSISTENCE_AOF_CURRENT_REWRITE_TIME_SEC_LABEL, common::Value::TYPE_INTEGER),
    Field(KEYDB_PERSISTENCE_AOF_LAST_DGREWRITE_STATUS_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_PERSISTENCE_AOF_LAST_WRITE_STATUS_LABEL, common::Value::TYPE_STRING)};

const std::vector<Field> kRedisStatsFields = {
    Field(KEYDB_STATS_TOTAL_CONNECTIONS_RECEIVED_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_TOTAL_COMMANDS_PROCESSED_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_INSTANTANEOUS_OPS_PER_SEC_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_REJECTED_CONNECTIONS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_SYNC_FULL_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_SYNC_PARTIAL_OK_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_SYNC_PARTIAL_ERR_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_EXPIRED_KEYS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_EVICTED_KEYS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_KEYSPACE_HITS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_KEYSPACE_MISSES_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_PUBSUB_CHANNELS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_PUBSUB_PATTERNS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_LATEST_FORK_USEC_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_MIGRATE_CACHED_SOCKETS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_SLAVE_EXPIRES_TRAKED_KEYS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_ACTIVE_DEFRAG_HITS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_ACTIVE_DEFRAG_MISSES_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_ACTIVE_DEFRAG_KEY_HITS_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_STATS_ACTIVE_DEFRAG_KEY_LABEL, common::Value::TYPE_UINTEGER)};

const std::vector<Field> kRedisReplicationFields = {
    Field(KEYDB_REPLICATION_ROLE_LABEL, common::Value::TYPE_STRING),
    Field(KEYDB_REPLICATION_CONNECTED_SLAVES_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_REPLICATION_MASTER_REPL_OFFSET_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_REPLICATION_BACKLOG_ACTIVE_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_REPLICATION_BACKLOG_SIZE_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_REPLICATION_BACKLOG_FIRST_BYTE_OFFSET_LABEL, common::Value::TYPE_UINTEGER),
    Field(KEYDB_REPLICATION_BACKLOG_HISTEN_LABEL, common::Value::TYPE_UINTEGER)};

const std::vector<Field> kRedisCpuFields = {Field(KEYDB_CPU_USED_CPU_SYS_LABEL, common::Value::TYPE_DOUBLE),
                                            Field(KEYDB_CPU_USED_CPU_USER_LABEL, common::Value::TYPE_DOUBLE),
                                            Field(KEYDB_CPU_USED_CPU_SYS_CHILDREN_LABEL, common::Value::TYPE_DOUBLE),
                                            Field(KEYDB_CPU_USED_CPU_USER_CHILDREN_LABEL, common::Value::TYPE_DOUBLE),
                                            Field(KEYDB_CPU_SERVER_THREADS_LABEL, common::Value::TYPE_UINTEGER),
                                            Field(KEYDB_CPU_LONG_LOCK_WAITS_LABEL, common::Value::TYPE_UINTEGER)};

const std::vector<Field> kRedisClusterFields = {
    Field(KEYDB_CLUSTER_CLUSTER_ENABLED_LABEL, common::Value::TYPE_UINTEGER)};

const std::vector<Field> kRedisKeyspaceFields = {};

std::ostream& operator<<(std::ostream& out, const ServerInfo::Server& value) {
  return out << KEYDB_SERVER_VERSION_LABEL COLON_STR << value.redis_version_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_GIT_SHA1_LABEL COLON_STR << value.redis_git_sha1_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_GIT_DIRTY_LABEL COLON_STR << value.redis_git_dirty_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_MODE_LABEL COLON_STR << value.redis_mode_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_OS_LABEL COLON_STR << value.os_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_ARCH_BITS_LABEL COLON_STR << value.arch_bits_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_MULTIPLEXING_API_LABEL COLON_STR << value.multiplexing_api_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_GCC_VERSION_LABEL COLON_STR << value.gcc_version_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_PROCESS_ID_LABEL COLON_STR << value.process_id_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_RUN_ID_LABEL COLON_STR << value.run_id_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_TCP_PORT_LABEL COLON_STR << value.tcp_port_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_UPTIME_IN_SECONDS_LABEL COLON_STR << value.uptime_in_seconds_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_UPTIME_IN_DAYS_LABEL COLON_STR << value.uptime_in_days_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_HZ_LABEL COLON_STR << value.hz_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_LRU_CLOCK_LABEL COLON_STR << value.lru_clock_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_EXECUTABLE_LABEL COLON_STR << value.executable_ << KEYDB_INFO_MARKER
             << KEYDB_SERVER_CONFIG_FILE_LABEL COLON_STR << value.config_file_ << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Clients& value) {
  return out << KEYDB_CLIENTS_CONNECTED_CLIENTS_LABEL COLON_STR << value.connected_clients_ << KEYDB_INFO_MARKER
             << KEYDB_CLIENTS_CLIENT_LONGEST_OUTPUT_LIST_LABEL COLON_STR << value.client_longest_output_list_
             << KEYDB_INFO_MARKER << KEYDB_CLIENTS_CLIENT_BIGGEST_INPUT_BUF_LABEL COLON_STR
             << value.client_biggest_input_buf_ << KEYDB_INFO_MARKER << KEYDB_CLIENTS_BLOCKED_CLIENTS_LABEL COLON_STR
             << value.blocked_clients_ << KEYDB_INFO_MARKER << KEYDB_CLIENTS_THREAD_0_CLIENTS_LABEL COLON_STR
             << value.thread_0_clients_ << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Memory& value) {
  return out << KEYDB_MEMORY_USED_MEMORY_LABEL COLON_STR << value.used_memory_ << KEYDB_INFO_MARKER
             << KEYDB_MEMORY_USED_MEMORY_HUMAN_LABEL COLON_STR << value.used_memory_human_ << KEYDB_INFO_MARKER
             << KEYDB_MEMORY_USED_MEMORY_RSS_LABEL COLON_STR << value.used_memory_rss_ << KEYDB_INFO_MARKER
             << KEYDB_MEMORY_USED_MEMORY_PEAK_LABEL COLON_STR << value.used_memory_peak_ << KEYDB_INFO_MARKER
             << KEYDB_MEMORY_USED_MEMORY_PEAK_HUMAN_LABEL COLON_STR << value.used_memory_peak_human_
             << KEYDB_INFO_MARKER << KEYDB_MEMORY_USED_MEMORY_LUA_LABEL COLON_STR << value.used_memory_lua_
             << KEYDB_INFO_MARKER << KEYDB_MEMORY_MEM_FRAGMENTATION_RATIO_LABEL COLON_STR
             << value.mem_fragmentation_ratio_ << KEYDB_INFO_MARKER << KEYDB_MEMORY_MEM_ALLOCATOR_LABEL COLON_STR
             << value.mem_allocator_ << KEYDB_INFO_MARKER << KEYDB_MEMORY_ACTIVE_DEFRAG_RUNNING_LABEL COLON_STR
             << value.active_defrag_running_ << KEYDB_INFO_MARKER
             << KEYDB_MEMORY_LAZYFREE_PENDING_OBJECTS_LABEL COLON_STR << value.lazyfree_pending_objects_
             << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Persistence& value) {
  return out << KEYDB_PERSISTENCE_LOADING_LABEL COLON_STR << value.loading_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_RDB_CHANGES_SINCE_LAST_SAVE_LABEL COLON_STR << value.rdb_changes_since_last_save_
             << KEYDB_INFO_MARKER << KEYDB_PERSISTENCE_RDB_DGSAVE_IN_PROGRESS_LABEL COLON_STR
             << value.rdb_bgsave_in_progress_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_RDB_LAST_SAVE_TIME_LABEL COLON_STR << value.rdb_last_save_time_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_RDB_LAST_DGSAVE_STATUS_LABEL COLON_STR << value.rdb_last_bgsave_status_
             << KEYDB_INFO_MARKER << KEYDB_PERSISTENCE_RDB_LAST_DGSAVE_TIME_SEC_LABEL COLON_STR
             << value.rdb_last_bgsave_time_sec_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_RDB_CURRENT_DGSAVE_TIME_SEC_LABEL COLON_STR << value.rdb_current_bgsave_time_sec_
             << KEYDB_INFO_MARKER << KEYDB_PERSISTENCE_AOF_ENABLED_LABEL COLON_STR << value.aof_enabled_
             << KEYDB_INFO_MARKER << KEYDB_PERSISTENCE_AOF_REWRITE_IN_PROGRESS_LABEL COLON_STR
             << value.aof_rewrite_in_progress_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_AOF_REWRITE_SHEDULED_LABEL COLON_STR << value.aof_rewrite_scheduled_
             << KEYDB_INFO_MARKER << KEYDB_PERSISTENCE_AOF_LAST_REWRITE_TIME_SEC_LABEL COLON_STR
             << value.aof_last_rewrite_time_sec_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_AOF_CURRENT_REWRITE_TIME_SEC_LABEL COLON_STR << value.aof_current_rewrite_time_sec_
             << KEYDB_INFO_MARKER << KEYDB_PERSISTENCE_AOF_LAST_DGREWRITE_STATUS_LABEL COLON_STR
             << value.aof_last_bgrewrite_status_ << KEYDB_INFO_MARKER
             << KEYDB_PERSISTENCE_AOF_LAST_WRITE_STATUS_LABEL COLON_STR << value.aof_last_write_status_
             << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Stats& value) {
  return out << KEYDB_STATS_TOTAL_CONNECTIONS_RECEIVED_LABEL COLON_STR << value.total_connections_received_
             << KEYDB_INFO_MARKER << KEYDB_STATS_TOTAL_COMMANDS_PROCESSED_LABEL COLON_STR
             << value.total_commands_processed_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_INSTANTANEOUS_OPS_PER_SEC_LABEL COLON_STR << value.instantaneous_ops_per_sec_
             << KEYDB_INFO_MARKER << KEYDB_STATS_REJECTED_CONNECTIONS_LABEL COLON_STR << value.rejected_connections_
             << KEYDB_INFO_MARKER << KEYDB_STATS_SYNC_FULL_LABEL COLON_STR << value.sync_full_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_SYNC_PARTIAL_OK_LABEL COLON_STR << value.sync_partial_ok_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_SYNC_PARTIAL_ERR_LABEL COLON_STR << value.sync_partial_err_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_EXPIRED_KEYS_LABEL COLON_STR << value.expired_keys_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_EVICTED_KEYS_LABEL COLON_STR << value.evicted_keys_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_KEYSPACE_HITS_LABEL COLON_STR << value.keyspace_hits_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_KEYSPACE_MISSES_LABEL COLON_STR << value.keyspace_misses_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_PUBSUB_CHANNELS_LABEL COLON_STR << value.pubsub_channels_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_PUBSUB_PATTERNS_LABEL COLON_STR << value.pubsub_patterns_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_LATEST_FORK_USEC_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_MIGRATE_CACHED_SOCKETS_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_SLAVE_EXPIRES_TRAKED_KEYS_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_ACTIVE_DEFRAG_HITS_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_ACTIVE_DEFRAG_MISSES_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_ACTIVE_DEFRAG_KEY_HITS_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER
             << KEYDB_STATS_ACTIVE_DEFRAG_KEY_LABEL COLON_STR << value.latest_fork_usec_ << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Replication& value) {
  return out << KEYDB_REPLICATION_ROLE_LABEL COLON_STR << value.role_ << KEYDB_INFO_MARKER
             << KEYDB_REPLICATION_CONNECTED_SLAVES_LABEL COLON_STR << value.connected_slaves_ << KEYDB_INFO_MARKER
             << KEYDB_REPLICATION_MASTER_REPL_OFFSET_LABEL COLON_STR << value.master_repl_offset_ << KEYDB_INFO_MARKER
             << KEYDB_REPLICATION_BACKLOG_ACTIVE_LABEL COLON_STR << value.backlog_active_ << KEYDB_INFO_MARKER
             << KEYDB_REPLICATION_BACKLOG_SIZE_LABEL COLON_STR << value.backlog_size_ << KEYDB_INFO_MARKER
             << KEYDB_REPLICATION_BACKLOG_FIRST_BYTE_OFFSET_LABEL COLON_STR << value.backlog_first_byte_offset_
             << KEYDB_INFO_MARKER << KEYDB_REPLICATION_BACKLOG_HISTEN_LABEL COLON_STR << value.backlog_histen_
             << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Cpu& value) {
  return out << KEYDB_CPU_USED_CPU_SYS_LABEL COLON_STR << value.used_cpu_sys_ << KEYDB_INFO_MARKER
             << KEYDB_CPU_USED_CPU_USER_LABEL COLON_STR << value.used_cpu_user_ << KEYDB_INFO_MARKER
             << KEYDB_CPU_USED_CPU_SYS_CHILDREN_LABEL COLON_STR << value.used_cpu_sys_children_ << KEYDB_INFO_MARKER
             << KEYDB_CPU_USED_CPU_USER_CHILDREN_LABEL COLON_STR << value.used_cpu_user_children_ << KEYDB_INFO_MARKER;
}

std::ostream& operator<<(std::ostream& out, const ServerInfo::Cluster& value) {
  return out << KEYDB_CLUSTER_CLUSTER_ENABLED_LABEL << value.cluster_enabled_ << KEYDB_INFO_MARKER;
}

}  // namespace

std::vector<common::Value::Type> GetSupportedValueTypes(uint32_t server_version) {
  std::vector<common::Value::Type> result = {common::Value::TYPE_STRING, common::Value::TYPE_ARRAY,
                                             common::Value::TYPE_SET, common::Value::TYPE_ZSET,
                                             common::Value::TYPE_HASH};
#if defined(PRO_VERSION)
  if (server_version == UNDEFINED_SINCE || server_version >= PROJECT_VERSION_CHECK(4, 0, 0)) {
    static const auto json_type = JsonValue::TYPE_JSON;  // #FIXME
    result.push_back(json_type);
  }
#endif
  if (server_version == UNDEFINED_SINCE || server_version >= PROJECT_VERSION_CHECK(5, 0, 0)) {
    static const auto stream_type = StreamValue::TYPE_STREAM;  // #FIXME
    result.push_back(stream_type);
  }

  return result;
}

std::vector<info_field_t> GetInfoFields() {
  return {std::make_pair(KEYDB_SERVER_LABEL, kRedisServerFields),
          std::make_pair(KEYDB_CLIENTS_LABEL, kRedisClientFields),
          std::make_pair(KEYDB_MEMORY_LABEL, kRedisMemoryFields),
          std::make_pair(KEYDB_PERSISTENCE_LABEL, kRedisPersistenceFields),
          std::make_pair(KEYDB_STATS_LABEL, kRedisStatsFields),
          std::make_pair(KEYDB_REPLICATION_LABEL, kRedisReplicationFields),
          std::make_pair(KEYDB_CPU_LABEL, kRedisCpuFields),
          std::make_pair(KEYDB_CLUSTER_LABEL, kRedisClusterFields),
          std::make_pair(KEYDB_KEYSPACE_LABEL, kRedisKeyspaceFields)};
}

ServerInfo::Server::Server::Server()
    : redis_version_(),
      redis_git_sha1_(),
      redis_git_dirty_(),
      redis_mode_(),
      os_(),
      arch_bits_(0),
      multiplexing_api_(),
      gcc_version_(),
      process_id_(0),
      run_id_(),
      tcp_port_(0),
      uptime_in_seconds_(0),
      uptime_in_days_(0),
      hz_(0),
      lru_clock_(0),
      executable_(),
      config_file_() {}

ServerInfo::Server::Server(const std::string& server_text)
    : redis_version_(),
      redis_git_sha1_(),
      redis_git_dirty_(),
      redis_mode_(),
      os_(),
      arch_bits_(0),
      multiplexing_api_(),
      gcc_version_(),
      process_id_(0),
      run_id_(),
      tcp_port_(0),
      uptime_in_seconds_(0),
      uptime_in_days_(0),
      hz_(0),
      lru_clock_(0),
      executable_(),
      config_file_() {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = server_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = server_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_SERVER_VERSION_LABEL) {
      redis_version_ = value;
    } else if (field == KEYDB_SERVER_GIT_SHA1_LABEL) {
      redis_git_sha1_ = value;
    } else if (field == KEYDB_SERVER_GIT_DIRTY_LABEL) {
      redis_git_dirty_ = value;
    } else if (field == KEYDB_SERVER_MODE_LABEL) {
      redis_mode_ = value;
    } else if (field == KEYDB_SERVER_OS_LABEL) {
      os_ = value;
    } else if (field == KEYDB_SERVER_ARCH_BITS_LABEL) {
      uint32_t arch_bits;
      if (common::ConvertFromString(value, &arch_bits)) {
        arch_bits_ = arch_bits;
      }
    } else if (field == KEYDB_SERVER_MULTIPLEXING_API_LABEL) {
      multiplexing_api_ = value;
    } else if (field == KEYDB_SERVER_GCC_VERSION_LABEL) {
      gcc_version_ = value;
    } else if (field == KEYDB_SERVER_PROCESS_ID_LABEL) {
      uint32_t process_id;
      if (common::ConvertFromString(value, &process_id)) {
        process_id_ = process_id;
      }
    } else if (field == KEYDB_SERVER_RUN_ID_LABEL) {
      run_id_ = value;
    } else if (field == KEYDB_SERVER_TCP_PORT_LABEL) {
      uint32_t tcp_port;
      if (common::ConvertFromString(value, &tcp_port)) {
        tcp_port_ = tcp_port;
      }
    } else if (field == KEYDB_SERVER_UPTIME_IN_SECONDS_LABEL) {
      uint32_t uptime_in_seconds;
      if (common::ConvertFromString(value, &uptime_in_seconds)) {
        uptime_in_seconds_ = uptime_in_seconds;
      }
    } else if (field == KEYDB_SERVER_UPTIME_IN_DAYS_LABEL) {
      uint32_t uptime_in_days;
      if (common::ConvertFromString(value, &uptime_in_days)) {
        uptime_in_days_ = uptime_in_days;
      }
    } else if (field == KEYDB_SERVER_HZ_LABEL) {
      uint32_t hz;
      if (common::ConvertFromString(value, &hz)) {
        hz_ = hz;
      }
    } else if (field == KEYDB_SERVER_LRU_CLOCK_LABEL) {
      uint32_t lru_clock;
      if (common::ConvertFromString(value, &lru_clock)) {
        lru_clock_ = lru_clock;
      }
    } else if (field == KEYDB_SERVER_EXECUTABLE_LABEL) {
      executable_ = value;
    } else if (field == KEYDB_SERVER_CONFIG_FILE_LABEL) {
      config_file_ = value;
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Server::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return common::Value::CreateStringValueFromBasicString(redis_version_);
    case 1:
      return common::Value::CreateStringValueFromBasicString(redis_git_sha1_);
    case 2:
      return common::Value::CreateStringValueFromBasicString(redis_git_dirty_);
    case 3:
      return common::Value::CreateStringValueFromBasicString(redis_build_id_);
    case 4:
      return common::Value::CreateStringValueFromBasicString(redis_mode_);
    case 5:
      return common::Value::CreateStringValueFromBasicString(os_);
    case 6:
      return new common::FundamentalValue(arch_bits_);
    case 7:
      return common::Value::CreateStringValueFromBasicString(multiplexing_api_);
    case 8:
      return common::Value::CreateStringValueFromBasicString(gcc_version_);
    case 9:
      return new common::FundamentalValue(process_id_);
    case 10:
      return common::Value::CreateStringValueFromBasicString(run_id_);
    case 11:
      return new common::FundamentalValue(tcp_port_);
    case 12:
      return new common::FundamentalValue(uptime_in_seconds_);
    case 13:
      return new common::FundamentalValue(uptime_in_days_);
    case 14:
      return new common::FundamentalValue(hz_);
    case 15:
      return new common::FundamentalValue(lru_clock_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Clients::Clients()
    : connected_clients_(0),
      client_longest_output_list_(0),
      client_biggest_input_buf_(0),
      blocked_clients_(0),
      thread_0_clients_(0) {}

ServerInfo::Clients::Clients(const std::string& client_text)
    : connected_clients_(0),
      client_longest_output_list_(0),
      client_biggest_input_buf_(0),
      blocked_clients_(0),
      thread_0_clients_(0) {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = client_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = client_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_CLIENTS_CONNECTED_CLIENTS_LABEL) {
      uint32_t connected_clients;
      if (common::ConvertFromString(value, &connected_clients)) {
        connected_clients_ = connected_clients;
      }
    } else if (field == KEYDB_CLIENTS_CLIENT_LONGEST_OUTPUT_LIST_LABEL) {
      uint32_t client_longest_output_list;
      if (common::ConvertFromString(value, &client_longest_output_list)) {
        client_longest_output_list_ = client_longest_output_list;
      }
    } else if (field == KEYDB_CLIENTS_CLIENT_BIGGEST_INPUT_BUF_LABEL) {
      uint32_t client_biggest_input_buf;
      if (common::ConvertFromString(value, &client_biggest_input_buf)) {
        client_biggest_input_buf_ = client_biggest_input_buf;
      }
    } else if (field == KEYDB_CLIENTS_BLOCKED_CLIENTS_LABEL) {
      uint32_t blocked_clients;
      if (common::ConvertFromString(value, &blocked_clients)) {
        blocked_clients_ = blocked_clients;
      }
    } else if (field == KEYDB_CLIENTS_THREAD_0_CLIENTS_LABEL) {
      uint32_t thread_0_clients;
      if (common::ConvertFromString(value, &thread_0_clients)) {
        thread_0_clients_ = thread_0_clients;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Clients::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(connected_clients_);
    case 1:
      return new common::FundamentalValue(client_longest_output_list_);
    case 2:
      return new common::FundamentalValue(client_biggest_input_buf_);
    case 3:
      return new common::FundamentalValue(blocked_clients_);
    case 4:
      return new common::FundamentalValue(thread_0_clients_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Memory::Memory()
    : used_memory_(0),
      used_memory_human_(),
      used_memory_rss_(0),
      used_memory_peak_(0),
      used_memory_peak_human_(),
      used_memory_lua_(0),
      mem_fragmentation_ratio_(0),
      mem_allocator_(),
      active_defrag_running_(0),
      lazyfree_pending_objects_(0) {}

ServerInfo::Memory::Memory(const std::string& memory_text)
    : used_memory_(0),
      used_memory_human_(),
      used_memory_rss_(0),
      used_memory_peak_(0),
      used_memory_peak_human_(),
      used_memory_lua_(0),
      mem_fragmentation_ratio_(0),
      mem_allocator_(),
      active_defrag_running_(0),
      lazyfree_pending_objects_(0) {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = memory_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = memory_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_MEMORY_USED_MEMORY_LABEL) {
      uint32_t used_memory;
      if (common::ConvertFromString(value, &used_memory)) {
        used_memory_ = used_memory;
      }
    } else if (field == KEYDB_MEMORY_USED_MEMORY_HUMAN_LABEL) {
      used_memory_human_ = value;
    } else if (field == KEYDB_MEMORY_USED_MEMORY_RSS_LABEL) {
      uint32_t used_memory_rss;
      if (common::ConvertFromString(value, &used_memory_rss)) {
        used_memory_rss_ = used_memory_rss;
      }
    } else if (field == KEYDB_MEMORY_USED_MEMORY_PEAK_LABEL) {
      uint32_t used_memory_peak;
      if (common::ConvertFromString(value, &used_memory_peak)) {
        used_memory_peak_ = used_memory_peak;
      }
    } else if (field == KEYDB_MEMORY_USED_MEMORY_PEAK_HUMAN_LABEL) {
      used_memory_peak_human_ = value;
    } else if (field == KEYDB_MEMORY_USED_MEMORY_LUA_LABEL) {
      uint32_t used_memory_lua;
      if (common::ConvertFromString(value, &used_memory_lua)) {
        used_memory_lua_ = used_memory_lua;
      }
    } else if (field == KEYDB_MEMORY_MEM_FRAGMENTATION_RATIO_LABEL) {
      float mem_fragmentation_ratio;
      if (common::ConvertFromString(value, &mem_fragmentation_ratio)) {
        mem_fragmentation_ratio_ = mem_fragmentation_ratio;
      }
    } else if (field == KEYDB_MEMORY_MEM_ALLOCATOR_LABEL) {
      mem_allocator_ = value;
    } else if (field == KEYDB_MEMORY_ACTIVE_DEFRAG_RUNNING_LABEL) {
      uint32_t active_defrag_running;
      if (common::ConvertFromString(value, &active_defrag_running)) {
        active_defrag_running_ = active_defrag_running;
      }
    } else if (field == KEYDB_MEMORY_LAZYFREE_PENDING_OBJECTS_LABEL) {
      uint32_t lazyfree_pending_objects;
      if (common::ConvertFromString(value, &lazyfree_pending_objects)) {
        lazyfree_pending_objects_ = lazyfree_pending_objects;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Memory::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(used_memory_);
    case 1:
      return common::Value::CreateStringValueFromBasicString(used_memory_human_);
    case 2:
      return new common::FundamentalValue(used_memory_rss_);
    case 3:
      return new common::FundamentalValue(used_memory_peak_);
    case 4:
      return common::Value::CreateStringValueFromBasicString(used_memory_peak_human_);
    case 5:
      return new common::FundamentalValue(used_memory_lua_);
    case 6:
      return new common::FundamentalValue(mem_fragmentation_ratio_);
    case 7:
      return common::Value::CreateStringValueFromBasicString(mem_allocator_);
    case 8:
      return new common::FundamentalValue(active_defrag_running_);
    case 9:
      return new common::FundamentalValue(lazyfree_pending_objects_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Persistence::Persistence()
    : loading_(0),
      rdb_changes_since_last_save_(0),
      rdb_bgsave_in_progress_(0),
      rdb_last_save_time_(0),
      rdb_last_bgsave_status_(),
      rdb_last_bgsave_time_sec_(0),
      rdb_current_bgsave_time_sec_(0),
      aof_enabled_(0),
      aof_rewrite_in_progress_(0),
      aof_rewrite_scheduled_(0),
      aof_last_rewrite_time_sec_(0),
      aof_current_rewrite_time_sec_(0),
      aof_last_bgrewrite_status_(),
      aof_last_write_status_() {}

ServerInfo::Persistence::Persistence(const std::string& persistence_text)
    : loading_(0),
      rdb_changes_since_last_save_(0),
      rdb_bgsave_in_progress_(0),
      rdb_last_save_time_(0),
      rdb_last_bgsave_status_(),
      rdb_last_bgsave_time_sec_(0),
      rdb_current_bgsave_time_sec_(0),
      aof_enabled_(0),
      aof_rewrite_in_progress_(0),
      aof_rewrite_scheduled_(0),
      aof_last_rewrite_time_sec_(0),
      aof_current_rewrite_time_sec_(0),
      aof_last_bgrewrite_status_(),
      aof_last_write_status_() {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = persistence_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = persistence_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_PERSISTENCE_LOADING_LABEL) {
      uint32_t loading;
      if (common::ConvertFromString(value, &loading)) {
        loading_ = loading;
      }
    } else if (field == KEYDB_PERSISTENCE_RDB_CHANGES_SINCE_LAST_SAVE_LABEL) {
      uint32_t rdb_changes_since_last_save;
      if (common::ConvertFromString(value, &rdb_changes_since_last_save)) {
        rdb_changes_since_last_save_ = rdb_changes_since_last_save;
      }
    } else if (field == KEYDB_PERSISTENCE_RDB_DGSAVE_IN_PROGRESS_LABEL) {
      uint32_t rdb_bgsave_in_progress;
      if (common::ConvertFromString(value, &rdb_bgsave_in_progress)) {
        rdb_bgsave_in_progress_ = rdb_bgsave_in_progress;
      }
    } else if (field == KEYDB_PERSISTENCE_RDB_LAST_SAVE_TIME_LABEL) {
      uint32_t rdb_last_save_time;
      if (common::ConvertFromString(value, &rdb_last_save_time)) {
        rdb_last_save_time_ = rdb_last_save_time;
      }
    } else if (field == KEYDB_PERSISTENCE_RDB_LAST_DGSAVE_STATUS_LABEL) {
      rdb_last_bgsave_status_ = value;
    } else if (field == KEYDB_PERSISTENCE_RDB_LAST_DGSAVE_TIME_SEC_LABEL) {
      int rdb_last_bgsave_time_sec;
      if (common::ConvertFromString(value, &rdb_last_bgsave_time_sec)) {
        rdb_last_bgsave_time_sec_ = rdb_last_bgsave_time_sec;
      }
    } else if (field == KEYDB_PERSISTENCE_RDB_CURRENT_DGSAVE_TIME_SEC_LABEL) {
      int rdb_current_bgsave_time_sec;
      if (common::ConvertFromString(value, &rdb_current_bgsave_time_sec)) {
        rdb_current_bgsave_time_sec_ = rdb_current_bgsave_time_sec;
      }
    } else if (field == KEYDB_PERSISTENCE_AOF_ENABLED_LABEL) {
      uint32_t aof_enabled;
      if (common::ConvertFromString(value, &aof_enabled)) {
        aof_enabled_ = aof_enabled;
      }
    } else if (field == KEYDB_PERSISTENCE_AOF_REWRITE_IN_PROGRESS_LABEL) {
      uint32_t aof_rewrite_in_progress;
      if (common::ConvertFromString(value, &aof_rewrite_in_progress)) {
        aof_rewrite_in_progress_ = aof_rewrite_in_progress;
      }
    } else if (field == KEYDB_PERSISTENCE_AOF_REWRITE_SHEDULED_LABEL) {
      uint32_t aof_rewrite_scheduled;
      if (common::ConvertFromString(value, &aof_rewrite_scheduled)) {
        aof_rewrite_scheduled_ = aof_rewrite_scheduled;
      }
    } else if (field == KEYDB_PERSISTENCE_AOF_LAST_REWRITE_TIME_SEC_LABEL) {
      int aof_last_rewrite_time_sec;
      if (common::ConvertFromString(value, &aof_last_rewrite_time_sec)) {
        aof_last_rewrite_time_sec_ = aof_last_rewrite_time_sec;
      }
    } else if (field == KEYDB_PERSISTENCE_AOF_CURRENT_REWRITE_TIME_SEC_LABEL) {
      int aof_current_rewrite_time_sec;
      if (common::ConvertFromString(value, &aof_current_rewrite_time_sec)) {
        aof_current_rewrite_time_sec_ = aof_current_rewrite_time_sec;
      }
    } else if (field == KEYDB_PERSISTENCE_AOF_LAST_DGREWRITE_STATUS_LABEL) {
      aof_last_bgrewrite_status_ = value;
    } else if (field == KEYDB_PERSISTENCE_AOF_LAST_WRITE_STATUS_LABEL) {
      aof_last_write_status_ = value;
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Persistence::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(loading_);
    case 1:
      return new common::FundamentalValue(rdb_changes_since_last_save_);
    case 2:
      return new common::FundamentalValue(rdb_bgsave_in_progress_);
    case 3:
      return new common::FundamentalValue(rdb_last_save_time_);
    case 4:
      return common::Value::CreateStringValueFromBasicString(rdb_last_bgsave_status_);
    case 5:
      return new common::FundamentalValue(rdb_last_bgsave_time_sec_);
    case 6:
      return new common::FundamentalValue(rdb_current_bgsave_time_sec_);
    case 7:
      return new common::FundamentalValue(aof_enabled_);
    case 8:
      return new common::FundamentalValue(aof_rewrite_in_progress_);
    case 9:
      return new common::FundamentalValue(aof_rewrite_scheduled_);
    case 10:
      return new common::FundamentalValue(aof_last_rewrite_time_sec_);
    case 11:
      return new common::FundamentalValue(aof_current_rewrite_time_sec_);
    case 12:
      return common::Value::CreateStringValueFromBasicString(aof_last_bgrewrite_status_);
    case 13:
      return common::Value::CreateStringValueFromBasicString(aof_last_write_status_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Stats::Stats()
    : total_connections_received_(0),
      total_commands_processed_(0),
      instantaneous_ops_per_sec_(0),
      rejected_connections_(0),
      sync_full_(0),
      sync_partial_ok_(0),
      sync_partial_err_(0),
      expired_keys_(0),
      evicted_keys_(0),
      keyspace_hits_(0),
      keyspace_misses_(0),
      pubsub_channels_(0),
      pubsub_patterns_(0),
      latest_fork_usec_(0),
      migrate_cached_sockets_(0),
      slave_expires_tracked_keys_(0),
      active_defrag_hits_(0),
      active_defrag_misses_(0),
      active_defrag_key_hits_(0),
      active_defrag_key_misses_(0) {}

ServerInfo::Stats::Stats(const std::string& stats_text)
    : total_connections_received_(0),
      total_commands_processed_(0),
      instantaneous_ops_per_sec_(0),
      rejected_connections_(0),
      sync_full_(0),
      sync_partial_ok_(0),
      sync_partial_err_(0),
      expired_keys_(0),
      evicted_keys_(0),
      keyspace_hits_(0),
      keyspace_misses_(0),
      pubsub_channels_(0),
      pubsub_patterns_(0),
      latest_fork_usec_(0),
      migrate_cached_sockets_(0),
      slave_expires_tracked_keys_(0),
      active_defrag_hits_(0),
      active_defrag_misses_(0),
      active_defrag_key_hits_(0),
      active_defrag_key_misses_(0) {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = stats_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = stats_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_STATS_TOTAL_CONNECTIONS_RECEIVED_LABEL) {
      uint32_t total_connections_received;
      if (common::ConvertFromString(value, &total_connections_received)) {
        total_connections_received_ = total_connections_received;
      }
    } else if (field == KEYDB_STATS_TOTAL_COMMANDS_PROCESSED_LABEL) {
      uint32_t total_commands_processed;
      if (common::ConvertFromString(value, &total_commands_processed)) {
        total_commands_processed_ = total_commands_processed;
      }
    } else if (field == KEYDB_STATS_INSTANTANEOUS_OPS_PER_SEC_LABEL) {
      uint32_t instantaneous_ops_per_sec;
      if (common::ConvertFromString(value, &instantaneous_ops_per_sec)) {
        instantaneous_ops_per_sec_ = instantaneous_ops_per_sec;
      }
    } else if (field == KEYDB_STATS_REJECTED_CONNECTIONS_LABEL) {
      uint32_t rejected_connections;
      if (common::ConvertFromString(value, &rejected_connections)) {
        rejected_connections_ = rejected_connections;
      }
    } else if (field == KEYDB_STATS_SYNC_FULL_LABEL) {
      uint32_t sync_full;
      if (common::ConvertFromString(value, &sync_full)) {
        sync_full_ = sync_full;
      }
    } else if (field == KEYDB_STATS_SYNC_PARTIAL_OK_LABEL) {
      uint32_t sync_partial_ok;
      if (common::ConvertFromString(value, &sync_partial_ok)) {
        sync_partial_ok_ = sync_partial_ok;
      }
    } else if (field == KEYDB_STATS_SYNC_PARTIAL_ERR_LABEL) {
      uint32_t sync_partial_err;
      if (common::ConvertFromString(value, &sync_partial_err)) {
        sync_partial_err_ = sync_partial_err;
      }
    } else if (field == KEYDB_STATS_EXPIRED_KEYS_LABEL) {
      uint32_t expired_keys;
      if (common::ConvertFromString(value, &expired_keys)) {
        expired_keys_ = expired_keys;
      }
    } else if (field == KEYDB_STATS_EVICTED_KEYS_LABEL) {
      uint32_t evicted_keys;
      if (common::ConvertFromString(value, &evicted_keys)) {
        evicted_keys_ = evicted_keys;
      }
    } else if (field == KEYDB_STATS_KEYSPACE_HITS_LABEL) {
      uint32_t keyspace_hits;
      if (common::ConvertFromString(value, &keyspace_hits)) {
        keyspace_hits_ = keyspace_hits;
      }
    } else if (field == KEYDB_STATS_KEYSPACE_MISSES_LABEL) {
      uint32_t keyspace_misses;
      if (common::ConvertFromString(value, &keyspace_misses)) {
        keyspace_misses_ = keyspace_misses;
      }
    } else if (field == KEYDB_STATS_PUBSUB_CHANNELS_LABEL) {
      uint32_t pubsub_channels;
      if (common::ConvertFromString(value, &pubsub_channels)) {
        pubsub_channels_ = pubsub_channels;
      }
    } else if (field == KEYDB_STATS_PUBSUB_PATTERNS_LABEL) {
      uint32_t pubsub_patterns;
      if (common::ConvertFromString(value, &pubsub_patterns)) {
        pubsub_patterns_ = pubsub_patterns;
      }
    } else if (field == KEYDB_STATS_LATEST_FORK_USEC_LABEL) {
      uint32_t latest_fork_usec;
      if (common::ConvertFromString(value, &latest_fork_usec)) {
        latest_fork_usec_ = latest_fork_usec;
      }
    } else if (field == KEYDB_STATS_MIGRATE_CACHED_SOCKETS_LABEL) {
      uint32_t migrate_cached_sockets;
      if (common::ConvertFromString(value, &migrate_cached_sockets)) {
        migrate_cached_sockets_ = migrate_cached_sockets;
      }
    } else if (field == KEYDB_STATS_SLAVE_EXPIRES_TRAKED_KEYS_LABEL) {
      uint32_t slave_expires_tracked_keys;
      if (common::ConvertFromString(value, &slave_expires_tracked_keys)) {
        slave_expires_tracked_keys_ = slave_expires_tracked_keys;
      }
    } else if (field == KEYDB_STATS_ACTIVE_DEFRAG_HITS_LABEL) {
      uint32_t active_defrag_hits;
      if (common::ConvertFromString(value, &active_defrag_hits)) {
        active_defrag_hits_ = active_defrag_hits;
      }
    } else if (field == KEYDB_STATS_ACTIVE_DEFRAG_MISSES_LABEL) {
      uint32_t active_defrag_misses;
      if (common::ConvertFromString(value, &active_defrag_misses)) {
        active_defrag_misses_ = active_defrag_misses;
      }
    } else if (field == KEYDB_STATS_ACTIVE_DEFRAG_KEY_HITS_LABEL) {
      uint32_t active_defrag_key_hits;
      if (common::ConvertFromString(value, &active_defrag_key_hits)) {
        active_defrag_key_hits_ = active_defrag_key_hits;
      }
    } else if (field == KEYDB_STATS_ACTIVE_DEFRAG_KEY_LABEL) {
      uint32_t active_defrag_key_misses;
      if (common::ConvertFromString(value, &active_defrag_key_misses)) {
        active_defrag_key_misses_ = active_defrag_key_misses;
      }
    }

    start = pos + 2;
  }
}

common::Value* ServerInfo::Stats::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(total_connections_received_);
    case 1:
      return new common::FundamentalValue(total_commands_processed_);
    case 2:
      return new common::FundamentalValue(instantaneous_ops_per_sec_);
    case 3:
      return new common::FundamentalValue(rejected_connections_);
    case 4:
      return new common::FundamentalValue(sync_full_);
    case 5:
      return new common::FundamentalValue(sync_partial_ok_);
    case 6:
      return new common::FundamentalValue(sync_partial_err_);
    case 7:
      return new common::FundamentalValue(expired_keys_);
    case 8:
      return new common::FundamentalValue(evicted_keys_);
    case 9:
      return new common::FundamentalValue(keyspace_hits_);
    case 10:
      return new common::FundamentalValue(keyspace_misses_);
    case 11:
      return new common::FundamentalValue(pubsub_channels_);
    case 12:
      return new common::FundamentalValue(pubsub_patterns_);
    case 13:
      return new common::FundamentalValue(latest_fork_usec_);
    case 14:
      return new common::FundamentalValue(migrate_cached_sockets_);
    case 15:
      return new common::FundamentalValue(slave_expires_tracked_keys_);
    case 16:
      return new common::FundamentalValue(active_defrag_hits_);
    case 17:
      return new common::FundamentalValue(active_defrag_misses_);
    case 18:
      return new common::FundamentalValue(active_defrag_key_hits_);
    case 19:
      return new common::FundamentalValue(active_defrag_key_misses_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Replication::Replication()
    : role_(),
      connected_slaves_(0),
      master_repl_offset_(0),
      backlog_active_(0),
      backlog_size_(0),
      backlog_first_byte_offset_(0),
      backlog_histen_(0) {}

ServerInfo::Replication::Replication(const std::string& replication_text)
    : role_(),
      connected_slaves_(0),
      master_repl_offset_(0),
      backlog_active_(0),
      backlog_size_(0),
      backlog_first_byte_offset_(0),
      backlog_histen_(0) {
  size_t pos = 0;
  size_t start = 0;

  while ((pos = replication_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = replication_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_REPLICATION_ROLE_LABEL) {
      role_ = value;
    } else if (field == KEYDB_REPLICATION_CONNECTED_SLAVES_LABEL) {
      uint32_t connected_slaves;
      if (common::ConvertFromString(value, &connected_slaves)) {
        connected_slaves_ = connected_slaves;
      }
    } else if (field == KEYDB_REPLICATION_MASTER_REPL_OFFSET_LABEL) {
      uint32_t master_repl_offset;
      if (common::ConvertFromString(value, &master_repl_offset)) {
        master_repl_offset_ = master_repl_offset;
      }
    } else if (field == KEYDB_REPLICATION_BACKLOG_ACTIVE_LABEL) {
      uint32_t backlog_active;
      if (common::ConvertFromString(value, &backlog_active)) {
        backlog_active_ = backlog_active;
      }
    } else if (field == KEYDB_REPLICATION_BACKLOG_SIZE_LABEL) {
      uint32_t backlog_size;
      if (common::ConvertFromString(value, &backlog_size)) {
        backlog_size_ = backlog_size;
      }
    } else if (field == KEYDB_REPLICATION_BACKLOG_FIRST_BYTE_OFFSET_LABEL) {
      uint32_t backlog_first_byte_offset;
      if (common::ConvertFromString(value, &backlog_first_byte_offset)) {
        backlog_first_byte_offset_ = backlog_first_byte_offset;
      }
    } else if (field == KEYDB_REPLICATION_BACKLOG_HISTEN_LABEL) {
      uint32_t backlog_histen;
      if (common::ConvertFromString(value, &backlog_histen)) {
        backlog_histen_ = backlog_histen;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Replication::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return common::Value::CreateStringValueFromBasicString(role_);
    case 1:
      return new common::FundamentalValue(connected_slaves_);
    case 2:
      return new common::FundamentalValue(master_repl_offset_);
    case 3:
      return new common::FundamentalValue(backlog_active_);
    case 4:
      return new common::FundamentalValue(backlog_size_);
    case 5:
      return new common::FundamentalValue(backlog_first_byte_offset_);
    case 6:
      return new common::FundamentalValue(backlog_histen_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Cpu::Cpu()
    : used_cpu_sys_(0),
      used_cpu_user_(0),
      used_cpu_sys_children_(0),
      used_cpu_user_children_(0),
      server_threads_(0),
      long_lock_waits_(0) {}

ServerInfo::Cpu::Cpu(const std::string& cpu_text)
    : used_cpu_sys_(0),
      used_cpu_user_(0),
      used_cpu_sys_children_(0),
      used_cpu_user_children_(0),
      server_threads_(0),
      long_lock_waits_(0) {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = cpu_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = cpu_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == (KEYDB_CPU_USED_CPU_SYS_LABEL)) {
      float used_cpu_sys;
      if (common::ConvertFromString(value, &used_cpu_sys)) {
        used_cpu_sys_ = used_cpu_sys;
      }
    } else if (field == KEYDB_CPU_USED_CPU_USER_LABEL) {
      float used_cpu_user;
      if (common::ConvertFromString(value, &used_cpu_user)) {
        used_cpu_user_ = used_cpu_user;
      }
    } else if (field == KEYDB_CPU_USED_CPU_SYS_CHILDREN_LABEL) {
      float used_cpu_sys_children;
      if (common::ConvertFromString(value, &used_cpu_sys_children)) {
        used_cpu_sys_children_ = used_cpu_sys_children;
      }
    } else if (field == KEYDB_CPU_USED_CPU_USER_CHILDREN_LABEL) {
      float used_cpu_user_children;
      if (common::ConvertFromString(value, &used_cpu_user_children)) {
        used_cpu_user_children_ = used_cpu_user_children;
      }
    } else if (field == KEYDB_CPU_SERVER_THREADS_LABEL) {
      uint32_t server_threads;
      if (common::ConvertFromString(value, &server_threads)) {
        server_threads_ = server_threads;
      }
    } else if (field == KEYDB_CPU_LONG_LOCK_WAITS_LABEL) {
      uint32_t long_lock_waits;
      if (common::ConvertFromString(value, &long_lock_waits)) {
        long_lock_waits_ = long_lock_waits;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Cpu::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(used_cpu_sys_);
    case 1:
      return new common::FundamentalValue(used_cpu_user_);
    case 2:
      return new common::FundamentalValue(used_cpu_sys_children_);
    case 3:
      return new common::FundamentalValue(used_cpu_user_children_);
    case 4:
      return new common::FundamentalValue(server_threads_);
    case 5:
      return new common::FundamentalValue(long_lock_waits_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

ServerInfo::Cluster::Cluster() : cluster_enabled_(0) {}

ServerInfo::Cluster::Cluster(const std::string& cluster_text) : cluster_enabled_(0) {
  size_t pos = 0;
  size_t start = 0;
  while ((pos = cluster_text.find(KEYDB_INFO_MARKER, start)) != std::string::npos) {
    std::string line = cluster_text.substr(start, pos - start);
    size_t delem = line.find_first_of(':');
    std::string field = line.substr(0, delem);
    std::string value = line.substr(delem + 1);
    if (field == KEYDB_CLUSTER_CLUSTER_ENABLED_LABEL) {
      uint32_t cluster_enabled;
      if (common::ConvertFromString(value, &cluster_enabled)) {
        cluster_enabled_ = cluster_enabled;
      }
    }
    start = pos + 2;
  }
}

common::Value* ServerInfo::Cluster::GetValueByIndex(unsigned char index) const {
  switch (index) {
    case 0:
      return new common::FundamentalValue(cluster_enabled_);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

common::Value* ServerInfo::Keyspace::GetValueByIndex(unsigned char index) const {
  UNUSED(index);

  return nullptr;
}

ServerInfo::ServerInfo() : IServerInfo() {}

ServerInfo::ServerInfo(const Server& serv,
                       const Clients& clients,
                       const Memory& memory,
                       const Persistence& pers,
                       const Stats& stats,
                       const Replication& repl,
                       const Cpu& cpu,
                       const Cluster& cluster,
                       const Keyspace& key)
    : IServerInfo(),
      server_(serv),
      clients_(clients),
      memory_(memory),
      persistence_(pers),
      stats_(stats),
      replication_(repl),
      cpu_(cpu),
      cluster_(cluster),
      keysp_(key) {}

ServerInfo::ServerInfo(const std::string& content) {
  size_t j = 0;
  std::string word;
  size_t pos = 0;
  static const std::vector<core::info_field_t> fields = GetInfoFields();
  for (size_t i = 0; i < content.size(); ++i) {
    char ch = content[i];
    word += ch;
    if (word == fields[j].first) {
      if (j + 1 != fields.size()) {
        pos = content.find(fields[j + 1].first, pos);
      } else {
        break;
      }

      if (pos != std::string::npos) {
        std::string part = content.substr(i + 1, pos - i - 1);
        switch (j) {
          case 0:
            server_ = ServerInfo::Server(part);
            break;
          case 1:
            clients_ = ServerInfo::Clients(part);
            break;
          case 2:
            memory_ = ServerInfo::Memory(part);
            break;
          case 3:
            persistence_ = ServerInfo::Persistence(part);
            break;
          case 4:
            stats_ = ServerInfo::Stats(part);
            break;
          case 5:
            replication_ = ServerInfo::Replication(part);
            break;
          case 6:
            cpu_ = ServerInfo::Cpu(part);
            break;
          default:
            break;
        }
        i = pos - 1;
        ++j;
      }
      word.clear();
    }
  }
}

common::Value* ServerInfo::GetValueByIndexes(unsigned char property, unsigned char field) const {
  switch (property) {
    case 0:
      return server_.GetValueByIndex(field);
    case 1:
      return clients_.GetValueByIndex(field);
    case 2:
      return memory_.GetValueByIndex(field);
    case 3:
      return persistence_.GetValueByIndex(field);
    case 4:
      return stats_.GetValueByIndex(field);
    case 5:
      return replication_.GetValueByIndex(field);
    case 6:
      return cpu_.GetValueByIndex(field);
    case 7:
      return keysp_.GetValueByIndex(field);
    default:
      break;
  }

  NOTREACHED();
  return nullptr;
}

std::string ServerInfo::ToString() const {
  std::stringstream str;
  str << KEYDB_SERVER_LABEL KEYDB_INFO_MARKER << server_ << KEYDB_CLIENTS_LABEL KEYDB_INFO_MARKER << clients_
      << KEYDB_MEMORY_LABEL KEYDB_INFO_MARKER << memory_ << KEYDB_PERSISTENCE_LABEL KEYDB_INFO_MARKER << persistence_
      << KEYDB_STATS_LABEL KEYDB_INFO_MARKER << stats_ << KEYDB_REPLICATION_LABEL KEYDB_INFO_MARKER << replication_
      << KEYDB_CPU_LABEL KEYDB_INFO_MARKER << cpu_ << KEYDB_CLUSTER_LABEL KEYDB_INFO_MARKER << cluster_
      << KEYDB_KEYSPACE_LABEL KEYDB_INFO_MARKER;
  return str.str();
}

uint32_t ServerInfo::GetVersion() const {
  return common::ConvertVersionNumberFromString(server_.redis_version_);
}

}  // namespace keydb
}  // namespace core
}  // namespace fastonosql
