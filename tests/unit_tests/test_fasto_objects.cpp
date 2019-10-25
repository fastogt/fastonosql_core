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

#include <gtest/gtest.h>

#include <fastonosql/core/global.h>

TEST(FastoObject, LifeTime) {
  fastonosql::core::FastoObjectIPtr obj = fastonosql::core::FastoObject::CreateRoot(GEN_CMD_STRING("root"));
  obj.reset();
  fastonosql::core::FastoObject* ptr = obj.get();
  ASSERT_TRUE(ptr == NULL);
}

TEST(FastoObject, LifeTimeScope) {
  common::StringValue* obj = common::Value::CreateStringValue(GEN_CMD_STRING("Sasha"));
  {
    fastonosql::core::FastoObjectIPtr root = fastonosql::core::FastoObject::CreateRoot(GEN_CMD_STRING("root"));
    fastonosql::core::FastoObject* ptr = new fastonosql::core::FastoObject(root.get(), obj, "/n");
    root->AddChildren(ptr);
  }
}
