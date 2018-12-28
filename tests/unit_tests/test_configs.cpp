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

#include <gtest/gtest.h>

#include <fastonosql/core/config/local_config.h>
#include <fastonosql/core/config/remote_config.h>

#ifdef BUILD_WITH_REDIS
#include <fastonosql/core/db/redis/config.h>
#endif

#ifdef BUILD_WITH_MEMCACHED
#include <fastonosql/core/db/memcached/config.h>
#endif

#ifdef BUILD_WITH_SSDB
#include <fastonosql/core/db/ssdb/config.h>
#endif

#ifdef BUILD_WITH_PIKA
#include <fastonosql/core/db/pika/config.h>
#endif

#ifdef BUILD_WITH_DYNOMITE
#include <fastonosql/core/db/dynomite/config.h>
#endif

#ifdef BUILD_WITH_LEVELDB
#include <fastonosql/core/db/leveldb/config.h>
#endif

#ifdef BUILD_WITH_ROCKSDB
#include <fastonosql/core/db/rocksdb/config.h>
#endif

#ifdef BUILD_WITH_LMDB
#include <fastonosql/core/db/lmdb/config.h>
#endif

#ifdef BUILD_WITH_UNQLITE
#include <fastonosql/core/db/unqlite/config.h>
#endif

#ifdef BUILD_WITH_FORESTDB
#include <fastonosql/core/db/forestdb/config.h>
#endif

template <typename Conf>
void Checker(Conf conf) {
  fastonosql::core::config_args_t args = conf.ToArgs();

  std::string out;
  bool is_ok = fastonosql::core::ConvertToStringConfigArgs(args, &out);
  ASSERT_TRUE(is_ok);

  fastonosql::core::config_args_t largs;
  is_ok = fastonosql::core::ConvertToConfigArgsString(out, &largs);
  ASSERT_TRUE(is_ok);

  Conf tested;
  tested.Init(largs);
  ASSERT_EQ(conf, tested);
}

TEST(Connection, base) {
  fastonosql::core::BaseConfig base;
  Checker(base);

  base.delimiter = "D";
  Checker(base);
}

TEST(Connection, local) {
  fastonosql::core::LocalConfig local;
  Checker(local);

  local.db_path = "~/path";
  Checker(local);
}

TEST(Connection, remote) {
  fastonosql::core::RemoteConfig remote;
  Checker(remote);

  remote.host = common::net::HostAndPort::CreateLocalHost(1111);
  Checker(remote);
}

#ifdef BUILD_WITH_REDIS
TEST(Connection, redis) {
  fastonosql::core::redis::Config conf;
  Checker(conf);

  conf.hostsocket = "/tmp/sock";
  conf.db_num = 3;
  conf.auth = "pass";
  conf.is_ssl = true;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_MEMCACHED
TEST(Connection, memcached) {
  fastonosql::core::memcached::Config conf;
  Checker(conf);

  conf.user = "user";
  conf.password = "pass";
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_SSDB
TEST(Connection, ssdb) {
  fastonosql::core::ssdb::Config conf;
  Checker(conf);

  conf.auth = "pass";
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_PIKA
TEST(Connection, pika) {
  fastonosql::core::pika::Config conf;
  Checker(conf);

  conf.hostsocket = "/tmp/sock1";
  conf.db_num = 33;
  conf.auth = "pass1";
  conf.is_ssl = true;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_DYNOMITE
TEST(Connection, dynomite) {
  fastonosql::core::dynomite::Config conf;
  Checker(conf);

  conf.hostsocket = "/tmp/sock1";
  conf.db_num = 33;
  conf.auth = "pass1";
  conf.is_ssl = true;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_LEVELDB
TEST(Connection, leveldb) {
  fastonosql::core::leveldb::Config conf;
  Checker(conf);

  conf.comparator = fastonosql::core::leveldb::COMP_INDEXED_DB;
  conf.compression = fastonosql::core::leveldb::kSnappyCompression;
  conf.create_if_missing = true;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_ROCKSDB
TEST(Connection, rocksdb) {
  fastonosql::core::rocksdb::Config conf;
  Checker(conf);

  conf.comparator = fastonosql::core::rocksdb::COMP_REVERSE_BYTEWISE;
  conf.compression = fastonosql::core::rocksdb::kLZ4Compression;
  conf.create_if_missing = true;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_LMDB
TEST(Connection, lmdb) {
  fastonosql::core::lmdb::Config conf;
  Checker(conf);

  conf.env_flags = 33;
  conf.db_name = "some";
  conf.max_dbs = 11;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_UNQLITE
TEST(Connection, unqlite) {
  fastonosql::core::unqlite::Config conf;
  Checker(conf);

  conf.env_flags = 33;
  Checker(conf);
}
#endif

#ifdef BUILD_WITH_FORESTDB
TEST(Connection, forestdb) {
  fastonosql::core::forestdb::Config conf;
  Checker(conf);

  conf.db_name = "tort";
  Checker(conf);
}
#endif
