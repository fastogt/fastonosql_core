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

#include <fastonosql/core/macros.h>
#include <fastonosql/core/value.h>

TEST(Value, StringValue) {
  typedef common::Value::string_t string_value_t;
  const string_value_t origin = {'a', 'l', 'e', 'x'};
  common::Value* string = common::Value::CreateStringValue(origin);
  auto string_str = fastonosql::core::ConvertValue(string, SPACE_STR);
  ASSERT_EQ(origin, string_str);
  delete string;

  const string_value_t binary = {0, 1, 2, 3};
  string = common::Value::CreateStringValue(binary);
  auto binary_str = fastonosql::core::ConvertValue(string, SPACE_STR);
  ASSERT_EQ(binary, binary_str);
  delete string;
}
