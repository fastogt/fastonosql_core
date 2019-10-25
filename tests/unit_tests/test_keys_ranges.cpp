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

#include <fastonosql/core/basic_types.h>

TEST(Keys, Ranges) {
  namespace core = fastonosql::core;
  static const core::raw_key_t invalid_key = core::raw_key_t();
  bool in_range = core::IsKeyInRange(core::kRangeKeyStart, invalid_key);
  ASSERT_FALSE(in_range);
  in_range = core::IsKeyInEndRange(invalid_key);
  ASSERT_FALSE(in_range);

  static const core::raw_key_t akey = GEN_CMD_STRING("a");
  in_range = core::IsKeyInRange(core::kRangeKeyStart, akey);
  ASSERT_TRUE(in_range);

  in_range = core::IsKeyInRange(akey, akey);
  ASSERT_TRUE(in_range);

  in_range = core::IsKeyInEndRange(akey);
  ASSERT_TRUE(in_range);

  in_range = core::IsKeyInEndRange(akey, akey);
  ASSERT_TRUE(in_range);

  in_range = core::IsKeyInEndRange(akey);
  ASSERT_TRUE(in_range);

  static const core::raw_key_t bkey = GEN_CMD_STRING("b");
  in_range = core::IsKeyInRange(akey, bkey);
  ASSERT_TRUE(in_range);
  in_range = core::IsKeyInRange(bkey, akey);
  ASSERT_FALSE(in_range);

  in_range = core::IsKeyInEndRange(akey, bkey);
  ASSERT_TRUE(in_range);
  in_range = core::IsKeyInEndRange(bkey, akey);
  ASSERT_FALSE(in_range);

  static const core::raw_key_t ckey = GEN_CMD_STRING("c");
  in_range = core::IsKeyInRange(akey, bkey, ckey);
  ASSERT_TRUE(in_range);
  in_range = core::IsKeyInRange(bkey, ckey, akey);
  ASSERT_FALSE(in_range);
  in_range = core::IsKeyInRange(ckey, akey, bkey);
  ASSERT_FALSE(in_range);

  in_range = core::IsKeyInEndRange(ckey, bkey);
  ASSERT_FALSE(in_range);
  in_range = core::IsKeyInEndRange(bkey, ckey);
  ASSERT_TRUE(in_range);
  in_range = core::IsKeyInEndRange(ckey, akey);
  ASSERT_FALSE(in_range);
  in_range = core::IsKeyInEndRange(akey, ckey);
  ASSERT_TRUE(in_range);

  static const core::raw_key_t a1key = GEN_CMD_STRING("a1");
  in_range = core::IsKeyInRange(a1key, bkey, ckey);
  ASSERT_TRUE(in_range);
  in_range = core::IsKeyInRange(bkey, ckey, a1key);
  ASSERT_FALSE(in_range);
  in_range = core::IsKeyInRange(ckey, a1key, bkey);
  ASSERT_FALSE(in_range);
  in_range = core::IsKeyInRange(bkey, a1key, ckey);
  ASSERT_FALSE(in_range);
}
