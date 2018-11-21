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

#include <fastonosql/config.h>

#include <string>  // for string
#include <vector>

namespace fastonosql {
namespace core {

enum ConnectionType : uint8_t {
#if defined(HAVE_REDIS)
  REDIS = 0,
#endif
#if defined(HAVE_MEMCACHED)
  MEMCACHED = 1,
#endif
#if defined(HAVE_SSDB)
  SSDB = 2,
#endif
#if defined(HAVE_LEVELDB)
  LEVELDB = 3,
#endif
#if defined(HAVE_ROCKSDB)
  ROCKSDB = 4,
#endif
#if defined(HAVE_UNQLITE)
  UNQLITE = 5,
#endif
#if defined(HAVE_LMDB)
  LMDB = 6,
#endif
#if defined(HAVE_UPSCALEDB)
  UPSCALEDB = 7,
#endif
#if defined(HAVE_FORESTDB)
  FORESTDB = 8,
#endif
#if defined(HAVE_PIKA)
  PIKA = 9
#endif
};  // supported types

enum ServerType : uint8_t { MASTER = 0, SLAVE };
enum ServerState : uint8_t { SUP = 0, SDOWN };
enum ServerConnectionState : uint8_t { SCONNECTED = 0, SDISCONNECTED };
enum ServerMode : uint8_t { STANDALONE = 0, SENTINEL, CLUSTER };

enum ConnectionMode : uint8_t { InteractiveMode = 0 };

bool IsRedisCompatible(ConnectionType type);
bool IsRemoteType(ConnectionType type);
bool IsSupportTTLKeys(ConnectionType type);
bool IsLocalType(ConnectionType type);
bool IsCanSSHConnection(ConnectionType type);
bool IsCanCreateDatabase(ConnectionType type);
bool IsCanRemoveDatabase(ConnectionType type);
const char* ConnectionTypeToString(ConnectionType t);

template <ConnectionType conection_type>
struct ConnectionTraits {
  static const char* GetDBName() { return ConnectionTypeToString(conection_type); }
  static const char* GetBasedOn();
  static const char* GetVersionApi();
};

}  // namespace core
}  // namespace fastonosql

namespace common {
std::string ConvertToString(fastonosql::core::ServerType st);
bool ConvertFromString(const std::string& from, fastonosql::core::ServerType* out);

std::string ConvertToString(fastonosql::core::ServerState st);
bool ConvertFromString(const std::string& from, fastonosql::core::ServerState* out);

std::string ConvertToString(fastonosql::core::ServerMode md);
std::string ConvertToString(fastonosql::core::ConnectionMode t);
}  // namespace common
