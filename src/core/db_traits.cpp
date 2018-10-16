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

#include <fastonosql/core/db_traits.h>

#ifdef BUILD_WITH_REDIS
#include <fastonosql/core/db/redis/server_info.h>
#endif
#ifdef BUILD_WITH_MEMCACHED
#include <fastonosql/core/db/memcached/server_info.h>
#endif
#ifdef BUILD_WITH_SSDB
#include <fastonosql/core/db/ssdb/server_info.h>
#endif
#ifdef BUILD_WITH_LEVELDB
#include <fastonosql/core/db/leveldb/server_info.h>
#endif
#ifdef BUILD_WITH_ROCKSDB
#include <fastonosql/core/db/rocksdb/server_info.h>
#endif
#ifdef BUILD_WITH_UNQLITE
#include <fastonosql/core/db/unqlite/server_info.h>
#endif
#ifdef BUILD_WITH_LMDB
#include <fastonosql/core/db/lmdb/server_info.h>
#endif
#ifdef BUILD_WITH_UPSCALEDB
#include <fastonosql/core/db/upscaledb/server_info.h>
#endif
#ifdef BUILD_WITH_FORESTDB
#include <fastonosql/core/db/forestdb/server_info.h>
#endif
#ifdef BUILD_WITH_PIKA
#include <fastonosql/core/db/pika/server_info.h>
#endif

namespace fastonosql {
namespace core {

Field::Field(const std::string& name, common::Value::Type type) : name(name), type(type) {}

bool Field::IsIntegral() const {
  return common::Value::IsIntegral(type);
}

std::vector<common::Value::Type> GetSupportedValueTypes(ConnectionType type) {
#ifdef BUILD_WITH_REDIS
  if (type == REDIS) {
    return redis::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_MEMCACHED
  if (type == MEMCACHED) {
    return memcached::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_SSDB
  if (type == SSDB) {
    return ssdb::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_LEVELDB
  if (type == LEVELDB) {
    return leveldb::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_ROCKSDB
  if (type == ROCKSDB) {
    return rocksdb::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_UNQLITE
  if (type == UNQLITE) {
    return unqlite::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_LMDB
  if (type == LMDB) {
    return lmdb::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_UPSCALEDB
  if (type == UPSCALEDB) {
    return upscaledb::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_FORESTDB
  if (type == FORESTDB) {
    return forestdb::GetSupportedValueTypes();
  }
#endif
#ifdef BUILD_WITH_PIKA
  if (type == PIKA) {
    return pika::GetSupportedValueTypes();
  }
#endif
  NOTREACHED();
  return std::vector<common::Value::Type>();
}

std::vector<info_field_t> GetInfoFieldsFromType(ConnectionType type) {
#ifdef BUILD_WITH_REDIS
  if (type == REDIS) {
    return redis::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_MEMCACHED
  if (type == MEMCACHED) {
    return memcached::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_SSDB
  if (type == SSDB) {
    return ssdb::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_LEVELDB
  if (type == LEVELDB) {
    return leveldb::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_ROCKSDB
  if (type == ROCKSDB) {
    return rocksdb::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_UNQLITE
  if (type == UNQLITE) {
    return unqlite::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_LMDB
  if (type == LMDB) {
    return lmdb::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_UPSCALEDB
  if (type == UPSCALEDB) {
    return upscaledb::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_FORESTDB
  if (type == FORESTDB) {
    return forestdb::GetInfoFields();
  }
#endif
#ifdef BUILD_WITH_PIKA
  if (type == PIKA) {
    return pika::GetInfoFields();
  }
#endif
  NOTREACHED() << "Unknown type: " << type;
  return std::vector<info_field_t>();
}

}  // namespace core
}  // namespace fastonosql
