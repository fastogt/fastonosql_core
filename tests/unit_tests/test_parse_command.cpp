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

#include <fastonosql/core/types.h>

TEST(Parse, GetSet) {
  fastonosql::core::commands_args_t args;
  bool is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("GET name"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("GET"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("name"));
  ASSERT_EQ(args.size(), 2);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SET name alex"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SET"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("name"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("alex"));
  ASSERT_EQ(args.size(), 3);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SET array [\"1\", \"2\"]"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SET"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("array"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("[\"1\", \"2\"]"));
  ASSERT_EQ(args.size(), 3);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SET json {\"count\": 2}"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SET"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("json"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("{\"count\": 2}"));
  ASSERT_EQ(args.size(), 3);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SET \x11 \x12\x11"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SET"));
  ASSERT_EQ(args[1], fastonosql::core::command_buffer_t({17}));
  ASSERT_EQ(args[2], fastonosql::core::command_buffer_t({18, 17}));
  ASSERT_EQ(args.size(), 3);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SET space 'tra ta ta'"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SET"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("space"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("tra ta ta"));
  ASSERT_EQ(args.size(), 3);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SET 'space space' \"tra ta ta2\""), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SET"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("space space"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("tra ta ta2"));
  ASSERT_EQ(args.size(), 3);
}

TEST(Parse, Select) {
  fastonosql::core::commands_args_t args;
  bool is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("SELECT 0"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("SELECT"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("0"));
  ASSERT_EQ(args.size(), 2);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("select 100"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("select"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("100"));
  ASSERT_EQ(args.size(), 2);
}

TEST(Parse, Other) {
  fastonosql::core::commands_args_t args;
  bool is_ok =
      fastonosql::core::ParseCommandLine(GEN_CMD_STRING("ZUNIONSTORE tmp:price 1 product:price WEIGHTS 1"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("ZUNIONSTORE"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("tmp:price"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("1"));
  ASSERT_EQ(args[3], GEN_CMD_STRING("product:price"));
  ASSERT_EQ(args[4], GEN_CMD_STRING("WEIGHTS"));
  ASSERT_EQ(args[5], GEN_CMD_STRING("1"));
  ASSERT_EQ(args.size(), 6);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("ZREMRANGEBYSCORE tmp:price -INF (50"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("ZREMRANGEBYSCORE"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("tmp:price"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("-INF"));
  ASSERT_EQ(args[3], GEN_CMD_STRING("(50"));
  ASSERT_EQ(args.size(), 4);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("EVAL 'local cursor; [...]' 1 tmp:name cool"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("EVAL"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("local cursor; [...]"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("1"));
  ASSERT_EQ(args[3], GEN_CMD_STRING("tmp:name"));
  ASSERT_EQ(args[4], GEN_CMD_STRING("cool"));
  ASSERT_EQ(args.size(), 5);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("ZINTERSTORE tmp:result tmp:price tmp:name"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("ZINTERSTORE"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("tmp:result"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("tmp:price"));
  ASSERT_EQ(args[3], GEN_CMD_STRING("tmp:name"));
  ASSERT_EQ(args.size(), 4);

  is_ok = fastonosql::core::ParseCommandLine(GEN_CMD_STRING("ZRANGE tmp:result 0 -1"), &args);
  ASSERT_TRUE(is_ok);
  ASSERT_EQ(args[0], GEN_CMD_STRING("ZRANGE"));
  ASSERT_EQ(args[1], GEN_CMD_STRING("tmp:result"));
  ASSERT_EQ(args[2], GEN_CMD_STRING("0"));
  ASSERT_EQ(args[3], GEN_CMD_STRING("-1"));
  ASSERT_EQ(args.size(), 4);
}
