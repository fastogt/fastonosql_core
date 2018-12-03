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

#include <fastonosql/core/connection_types.h>

#include <common/macros.h>  // for NOTREACHED, SIZEOFMASS

namespace {
const char* kConnnectionType[] = {"Redis", "Memcached", "SSDB",     "LevelDB", "RocksDB",   "UnQLite",
                                  "LMDB",  "UpscaleDB", "ForestDB", "Pika",    "DynomiteDB"};
const char* kConnnectionMode[] = {"Interactive mode"};
const char* kServerType[] = {"Master", "Slave"};
const char* kServerState[] = {"Up", "Down"};
const char* kServerModes[] = {"Standalone", "Sentinel", "Cluster"};
}  // namespace

namespace fastonosql {
namespace core {

bool IsRedisCompatible(ConnectionType type) {
  return type == REDIS || type == PIKA || type == DYNOMITEDB;
}

bool IsRemoteType(ConnectionType type) {
  return type == REDIS || type == PIKA || type == DYNOMITEDB || type == MEMCACHED || type == SSDB;
}

bool IsSupportTTLKeys(ConnectionType type) {
  return type == REDIS || type == PIKA || type == DYNOMITEDB || type == MEMCACHED || type == SSDB;
}

bool IsLocalType(ConnectionType type) {
  return false
#if defined(BUILD_WITH_ROCKSDB)
         || type == ROCKSDB
#endif
#if defined(BUILD_WITH_LEVELDB)
         || type == LEVELDB
#endif
#if defined(BUILD_WITH_LMDB)
         || type == LMDB
#endif
#if defined(BUILD_WITH_UPSCALEDB)
         || type == UPSCALEDB
#endif
#if defined(BUILD_WITH_UNQLITE)
         || type == UNQLITE
#endif
#if defined(BUILD_WITH_FORESTDB)
         || type == FORESTDB
#endif
      ;
}

bool IsCanSSHConnection(ConnectionType type) {
  return IsRedisCompatible(type);
}

bool IsCanCreateDatabase(ConnectionType type) {
  return type == LMDB || type == FORESTDB || type == ROCKSDB;
}

bool IsCanRemoveDatabase(ConnectionType type) {
  return type == LMDB || type == FORESTDB || type == ROCKSDB;
}

const char* ConnectionTypeToString(ConnectionType t) {
  return kConnnectionType[t];
}

}  // namespace core
}  // namespace fastonosql

namespace common {

bool ConvertFromString(const std::string& from, fastonosql::core::ServerType* out) {
  if (!out) {
    return false;
  }

  for (size_t i = 0; i < SIZEOFMASS(kServerType); ++i) {
    if (from == kServerType[i]) {
      *out = static_cast<fastonosql::core::ServerType>(i);
      return true;
    }
  }

  DNOTREACHED();
  return false;
}

bool ConvertFromString(const std::string& from, fastonosql::core::ServerState* out) {
  for (size_t i = 0; i < SIZEOFMASS(kServerState); ++i) {
    if (from == kServerState[i]) {
      *out = static_cast<fastonosql::core::ServerState>(i);
      return true;
    }
  }

  DNOTREACHED();
  return false;
}

std::string ConvertToString(fastonosql::core::ServerType st) {
  return kServerType[st];
}

std::string ConvertToString(fastonosql::core::ServerState st) {
  return kServerState[st];
}

std::string ConvertToString(fastonosql::core::ServerMode md) {
  return kServerModes[md];
}

std::string ConvertToString(fastonosql::core::ConnectionMode t) {
  return kConnnectionMode[t];
}

}  // namespace common
