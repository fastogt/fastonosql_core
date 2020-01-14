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

#include <fastonosql/core/connection_types.h>

#include <common/macros.h>

namespace {
const char* kConnnectionType[] = {"Redis", "Memcached", "SSDB", "LevelDB",  "RocksDB", "UnQLite",
                                  "LMDB",  "ForestDB",  "Pika", "Dynomite", "KeyDB"};
const char* kConnnectionMode[] = {"Interactive mode"};
const char* kServerType[] = {"Master", "Slave"};
const char* kServerState[] = {"Up", "Down"};
const char* kServerModes[] = {"Standalone", "Sentinel", "Cluster"};
}  // namespace

namespace fastonosql {
namespace core {

bool IsRedisCompatible(ConnectionType type) {
#if defined(BUILD_WITH_REDIS)
  if (type == REDIS) {
    return true;
  }
#endif
#if defined(BUILD_WITH_PIKA)
  if (type == PIKA) {
    return true;
  }
#endif
#if defined(BUILD_WITH_DYNOMITE)
  if (type == DYNOMITE) {
    return true;
  }
#endif
#if defined(BUILD_WITH_KEYDB)
  if (type == KEYDB) {
    return true;
  }
#endif
  return false;
}

bool IsRemoteType(ConnectionType type) {
#if defined(BUILD_WITH_REDIS)
  if (type == REDIS) {
    return true;
  }
#endif
#if defined(BUILD_WITH_MEMCACHED)
  if (type == MEMCACHED) {
    return true;
  }
#endif
#if defined(BUILD_WITH_PIKA)
  if (type == PIKA) {
    return true;
  }
#endif
#if defined(BUILD_WITH_DYNOMITE)
  if (type == DYNOMITE) {
    return true;
  }
#endif
#if defined(BUILD_WITH_SSDB)
  if (type == SSDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_KEYDB)
  if (type == KEYDB) {
    return true;
  }
#endif
  return false;
}

bool IsSupportTTLKeys(ConnectionType type) {
#if defined(BUILD_WITH_REDIS)
  if (type == REDIS) {
    return true;
  }
#endif
#if defined(BUILD_WITH_PIKA)
  if (type == PIKA) {
    return true;
  }
#endif
#if defined(BUILD_WITH_DYNOMITE)
  if (type == DYNOMITE) {
    return true;
  }
#endif
#if defined(BUILD_WITH_SSDB)
  if (type == SSDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_KEYDB)
  if (type == KEYDB) {
    return true;
  }
#endif
  return false;
}

bool IsLocalType(ConnectionType type) {
#if defined(BUILD_WITH_ROCKSDB)
  if (type == ROCKSDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_LEVELDB)
  if (type == LEVELDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_LMDB)
  if (type == LMDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_UNQLITE)
  if (type == UNQLITE) {
    return true;
  }
#endif
#if defined(BUILD_WITH_FORESTDB)
  if (type == FORESTDB) {
    return true;
  }
#endif
  return false;
}

bool IsCanSSHConnection(ConnectionType type) {
  return IsRedisCompatible(type);
}

bool IsCanCreateDatabase(ConnectionType type) {
#if defined(BUILD_WITH_ROCKSDB)
  if (type == ROCKSDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_LMDB)
  if (type == LMDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_FORESTDB)
  if (type == FORESTDB) {
    return true;
  }
#endif
  return false;
}

bool IsCanRemoveDatabase(ConnectionType type) {
#if defined(BUILD_WITH_ROCKSDB)
  if (type == ROCKSDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_LMDB)
  if (type == LMDB) {
    return true;
  }
#endif
#if defined(BUILD_WITH_FORESTDB)
  if (type == FORESTDB) {
    return true;
  }
#endif
  return false;
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
