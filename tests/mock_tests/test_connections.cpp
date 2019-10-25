/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

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

#include <gmock/gmock.h>

#include <common/file_system/file_system.h>

#include <fastonosql/core/cdb_connection.h>

#ifdef BUILD_WITH_FORESTDB
#include <fastonosql/core/db/forestdb/db_connection.h>
#endif
#ifdef BUILD_WITH_LEVELDB
#include <fastonosql/core/db/leveldb/db_connection.h>
#endif
#ifdef BUILD_WITH_LMDB
#include <fastonosql/core/db/lmdb/db_connection.h>
#endif
#ifdef BUILD_WITH_ROCKSDB
#include <fastonosql/core/db/rocksdb/db_connection.h>
#endif
#ifdef BUILD_WITH_UNQLITE
#include <fastonosql/core/db/unqlite/db_connection.h>
#endif

namespace fastonosql {

template <typename NConnection, typename Config, core::ConnectionType ContType>
void CheckSetGet(core::CDBConnection<NConnection, Config, ContType>* db) {
  ASSERT_TRUE(db->IsConnected());
  core::NValue val(common::Value::CreateStringValueFromBasicString("test"));
  core::nkey_t key_str({'t', 'e', 's', 't'});
  core::NKey key(key_str);
  core::NDbKValue res1;
  common::Error err = db->Get(key, &res1);
  ASSERT_TRUE(err);

  core::NDbKValue kv(key, val);
  err = db->Set(kv);
  ASSERT_TRUE(!err);
  core::NDbKValue res2;
  err = db->Get(key, &res2);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(kv == res2);
  ASSERT_TRUE(db->IsConnected());

  core::NKeys want_delete = {key};
  core::NKeys deleted;
  err = db->Delete(want_delete, &deleted);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(want_delete == deleted);
  deleted = core::NKeys();
  err = db->Delete(want_delete, &deleted);
  ASSERT_TRUE(deleted.empty());  // must be
  ASSERT_TRUE(!err);
  core::NDbKValue res3;
  err = db->Get(key, &res3);
  ASSERT_TRUE(err);
  ASSERT_TRUE(db->IsConnected());
}

#ifdef BUILD_WITH_LEVELDB
TEST(Connection, leveldb) {
  core::leveldb::DBConnection db(nullptr);
  core::leveldb::Config lcfg;
  common::ErrnoError errn = common::file_system::remove_directory(lcfg.db_path, true);
  ASSERT_TRUE(!errn);

  lcfg.create_if_missing = false;
  common::Error err = db.Connect(lcfg);
  ASSERT_TRUE(err);
  ASSERT_TRUE(!db.IsConnected());
  errn = common::file_system::create_directory(lcfg.db_path, false);
  ASSERT_TRUE(!errn);
  err = db.Connect(lcfg);
  ASSERT_TRUE(err);
  ASSERT_TRUE(!db.IsConnected());

  lcfg.create_if_missing = true;
  err = db.Connect(lcfg);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(db.IsConnected());

  CheckSetGet(&db);

  err = db.Disconnect();
  ASSERT_TRUE(!err);
  ASSERT_TRUE(!db.IsConnected());

  errn = common::file_system::remove_directory(lcfg.db_path, true);
  ASSERT_TRUE(!errn);
}
#endif

#ifdef BUILD_WITH_ROCKSDB
TEST(Connection, rocksdb) {
  core::rocksdb::DBConnection db(nullptr);
  core::rocksdb::Config lcfg;
  common::ErrnoError errn = common::file_system::remove_directory(lcfg.db_path, true);
  ASSERT_TRUE(!errn);

  lcfg.create_if_missing = false;
  common::Error err = db.Connect(lcfg);
  ASSERT_TRUE(err);
  ASSERT_TRUE(!db.IsConnected());
  errn = common::file_system::create_directory(lcfg.db_path, false);
  ASSERT_TRUE(!errn);
  err = db.Connect(lcfg);
  ASSERT_TRUE(err);
  ASSERT_TRUE(!db.IsConnected());

  lcfg.create_if_missing = true;
  err = db.Connect(lcfg);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(db.IsConnected());

  CheckSetGet(&db);

  err = db.Disconnect();
  ASSERT_TRUE(!err);
  ASSERT_TRUE(!db.IsConnected());

  errn = common::file_system::remove_directory(lcfg.db_path, true);
  ASSERT_TRUE(!errn);
}
#endif

#ifdef BUILD_WITH_LMDB
TEST(Connection, lmdb) {
  core::lmdb::DBConnection db(nullptr);
  core::lmdb::Config lcfg;
  common::ErrnoError errn = common::file_system::remove_file(lcfg.db_path);
  ASSERT_TRUE(!errn);

  lcfg.SetReadOnlyDB(true);
  common::Error err = db.Connect(lcfg);
  ASSERT_TRUE(err);
  ASSERT_TRUE(!db.IsConnected());
  /*errn = common::file_system::create_node(lcfg.db_path);
  ASSERT_TRUE(!errn);
  err = db.Connect(lcfg);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(db.IsConnected());*/

  lcfg.SetReadOnlyDB(false);
  err = db.Connect(lcfg);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(db.IsConnected());
  CheckSetGet(&db);

  err = db.Disconnect();
  ASSERT_TRUE(!err);
  ASSERT_TRUE(!db.IsConnected());

  errn = common::file_system::remove_file(lcfg.db_path);
  ASSERT_TRUE(!errn);
}
#endif

#ifdef BUILD_WITH_UNQLITE
TEST(Connection, unqlite) {
  core::unqlite::DBConnection db(nullptr);
  core::unqlite::Config lcfg;
  lcfg.SetCreateIfMissingDB(true);  // workaround
  common::Error err = db.Connect(lcfg);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(db.IsConnected());

  CheckSetGet(&db);

  err = db.Disconnect();
  ASSERT_TRUE(!err);
  ASSERT_TRUE(!db.IsConnected());

  common::ErrnoError errn = common::file_system::remove_file(lcfg.db_path);
  ASSERT_TRUE(!errn);
}
#endif

#ifdef BUILD_WITH_FORESTDB
TEST(Connection, forestdb) {
  core::forestdb::DBConnection db(nullptr);
  core::forestdb::Config lcfg;
  common::Error err = db.Connect(lcfg);
  ASSERT_TRUE(!err);
  ASSERT_TRUE(db.IsConnected());

  CheckSetGet(&db);

  err = db.Disconnect();
  ASSERT_TRUE(!err);
  ASSERT_TRUE(!db.IsConnected());

  common::ErrnoError errn = common::file_system::remove_file(lcfg.db_path);
  ASSERT_TRUE(!errn);
}
#endif

}  // namespace fastonosql
