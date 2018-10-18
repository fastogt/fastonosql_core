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

#include <string>  // for string
#include <vector>

namespace fastonosql {
namespace core {

enum ConnectionType {
  REDIS = 0,
  MEMCACHED = 1,
  SSDB = 2,
  LEVELDB = 3,
  ROCKSDB = 4,
  UNQLITE = 5,
  LMDB = 6,
  UPSCALEDB = 7,
  FORESTDB = 8,
  PIKA = 9
};  // supported types

enum ServerType { MASTER = 0, SLAVE };
enum ServerState { SUP = 0, SDOWN };
enum ServerConnectionState { SCONNECTED = 0, SDISCONNECTED };
enum ServerMode { STANDALONE = 0, SENTINEL, CLUSTER };

extern const std::vector<ConnectionType> g_compiled_types;

enum ConnectionMode { InteractiveMode };

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
std::string ConvertToString(fastonosql::core::ConnectionType t);
bool ConvertFromString(const std::string& from, fastonosql::core::ConnectionType* out);

std::string ConvertToString(fastonosql::core::ServerType st);
bool ConvertFromString(const std::string& from, fastonosql::core::ServerType* out);

std::string ConvertToString(fastonosql::core::ServerState st);
bool ConvertFromString(const std::string& from, fastonosql::core::ServerState* out);

std::string ConvertToString(fastonosql::core::ServerMode md);
std::string ConvertToString(fastonosql::core::ConnectionMode t);
}  // namespace common
