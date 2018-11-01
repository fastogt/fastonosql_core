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

#include <fastonosql/core/types.h>

TEST(ReadableString, TextConversion) {
  fastonosql::core::ReadableString r;
  ASSERT_EQ(r.GetData(), fastonosql::core::readable_string_t());
  const fastonosql::core::readable_string_t valid = {'H', 'e', 'l', 'l', 'o'};
  r.SetData(valid);
  ASSERT_EQ(r.GetType(), fastonosql::core::ReadableString::TEXT_DATA);
  ASSERT_EQ(r.GetData(), valid);
  ASSERT_EQ(r.GetHumanReadable(), valid);

  const fastonosql::core::readable_string_t valid_with_rn = {'H', 'e', 'l', 'l', 'o', '\r', '\n'};
  r.SetData(valid_with_rn);
  ASSERT_EQ(r.GetType(), fastonosql::core::ReadableString::TEXT_DATA);
  ASSERT_EQ(r.GetData(), valid_with_rn);
  ASSERT_EQ(r.GetHumanReadable(), valid_with_rn);

  const fastonosql::core::readable_string_t valid_bin = {'H', 'e', 'l', 'l', 'o', 0, '1', '2', '3'};
  const fastonosql::core::readable_string_t valid_bin_str = {'\\', 'x', '4', '8', '\\', 'x', '6', '5', '\\', 'x', '6', 'c',
                                           '\\', 'x', '6', 'c', '\\', 'x', '6', 'f', '\\', 'x', '0', '0',
                                           '\\', 'x', '3', '1', '\\', 'x', '3', '2', '\\', 'x', '3', '3'};
  r.SetData(valid_bin);
  ASSERT_EQ(r.GetType(), fastonosql::core::ReadableString::BINARY_DATA);
  ASSERT_EQ(r.GetData(), valid_bin);
  ASSERT_EQ(r.GetHumanReadable(), valid_bin_str);
}
