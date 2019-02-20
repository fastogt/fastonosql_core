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

#include <vector>

#include <common/types.h>

#include <fastonosql/core/db_key.h>

namespace fastonosql {
namespace core {

class IDataBaseInfo : public common::ClonableBase<IDataBaseInfo> {
 public:
  typedef std::vector<NDbKValue> keys_container_t;

  db_name_t GetName() const;
  size_t GetDBKeysCount() const;
  void SetDBKeysCount(size_t size);
  size_t LoadedKeysCount() const;

  bool IsDefault() const;
  void SetIsDefault(bool is_def);

  ~IDataBaseInfo() override;

  keys_container_t GetKeys() const;
  void SetKeys(const keys_container_t& keys);
  void ClearKeys();

  bool RenameKey(const NKey& okey, const nkey_t& new_name) WARN_UNUSED_RESULT;
  bool InsertKey(const NDbKValue& key) WARN_UNUSED_RESULT;  // true if inserted, false if updated
  bool UpdateKeyTTL(const NKey& key, ttl_t ttl) WARN_UNUSED_RESULT;
  bool RemoveKey(const NKey& key) WARN_UNUSED_RESULT;

  IDataBaseInfo* Clone() const override = 0;

 protected:
  IDataBaseInfo(const db_name_t& name, bool is_default, size_t dbkcount, const keys_container_t& keys);

 private:
  const db_name_t name_;
  bool is_default_;
  size_t db_kcount_;
  keys_container_t keys_;
};

typedef std::shared_ptr<IDataBaseInfo> IDataBaseInfoSPtr;

}  // namespace core
}  // namespace fastonosql
