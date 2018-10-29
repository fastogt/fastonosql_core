#include <gtest/gtest.h>

extern "C" {
#include "sds/sds_fasto.h"
}

#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include <string.h>

#include <common/convert2string.h>
#include <common/error.h>

#include <fastonosql/core/types.h>

TEST(sds, sdssplitargslong) {
  const std::string json = R"({
                             "array": [
                               1,
                               2,
                               3
                             ],
                             "boolean": true,
                             "null": null,
                             "number": 123,
                             "object": {
                                 "a": "b",
                                 "c": "d",
                                 "e": "f"
                               },
                             "string": "Hello World"
                           })";
  const std::string json2 = R"({
                             "array": [
                               1,
                               2,
                               3
                             ],
                             "boolean": true,
                             "null": null,
                             "number": 123,
                             "string": "Hello World"
                           })";
  json_object* obj = json_tokener_parse(json.c_str());
  ASSERT_TRUE(obj);
  json_object_put(obj);
  obj = json_tokener_parse(json2.c_str());
  ASSERT_TRUE(obj);
  json_object_put(obj);

  const std::string line = "-d \n -c -h 127.0.0.1 -p \t -j " + json + " -j2 " + json2;

  {
    const common::char_buffer_t lineb = common::ConvertToCharBytes(line);
    fastonosql::core::commands_args_t res;
    bool is_ok = fastonosql::core::ParseCommandLine(lineb, &res);
    ASSERT_TRUE(is_ok);
    ASSERT_EQ(res.size(), 11);
    ASSERT_EQ(res[0], GEN_CMD_STRING("-d"));
    ASSERT_EQ(res[1], GEN_CMD_STRING("\n"));
    ASSERT_EQ(res[2], GEN_CMD_STRING("-c"));
    ASSERT_EQ(res[3], GEN_CMD_STRING("-h"));
    ASSERT_EQ(res[4], GEN_CMD_STRING("127.0.0.1"));
    ASSERT_EQ(res[5], GEN_CMD_STRING("-p"));
    ASSERT_EQ(res[6], GEN_CMD_STRING("\t"));
    ASSERT_EQ(res[7], GEN_CMD_STRING("-j"));
    const char* jafter = res[8].data();
    ASSERT_STREQ(jafter, json.c_str());
    ASSERT_EQ(res[9], GEN_CMD_STRING("-j2"));
    const char* jafter2 = res[10].data();
    ASSERT_STREQ(jafter2, json2.c_str());
  }

  int argc = 0;
  sds* argv = sdssplitargslong(line.c_str(), &argc);
  ASSERT_TRUE(argv && argc == 11);
  if (argv) {
    ASSERT_STREQ(argv[0], "-d");
    ASSERT_STREQ(argv[1], "\n");
    ASSERT_STREQ(argv[2], "-c");
    ASSERT_STREQ(argv[3], "-h");
    ASSERT_STREQ(argv[4], "127.0.0.1");
    ASSERT_STREQ(argv[5], "-p");
    ASSERT_STREQ(argv[6], "\t");
    ASSERT_STREQ(argv[7], "-j");
    const char* jafter = argv[8];
    ASSERT_STREQ(jafter, json.c_str());
    ASSERT_STREQ(argv[9], "-j2");
    const char* jafter2 = argv[10];
    ASSERT_STREQ(jafter2, json2.c_str());
    sds revert = sdsjoinsdsstable(argv, argc);
    ASSERT_STREQ(line.c_str(), revert);
    sdsfree(revert);
    sdsfreesplitres(argv, argc);
  }

  const std::string line2 =
      "-f '/home/nico/.config/chromium/Default/Local Extension "
      "Settings/ncdlagniojmheiklojdcpdaeepochckl/000100.ldb' -d \n -ns";
  {
    const common::char_buffer_t lineb2 = common::ConvertToCharBytes(line2);
    fastonosql::core::commands_args_t res;
    bool is_ok = fastonosql::core::ParseCommandLine(lineb2, &res);
    ASSERT_TRUE(is_ok);
    ASSERT_EQ(res[0], GEN_CMD_STRING("-f"));
    ASSERT_EQ(res[1], GEN_CMD_STRING("/home/nico/.config/chromium/Default/Local Extension "
                                     "Settings/ncdlagniojmheiklojdcpdaeepochckl/000100.ldb"));
    ASSERT_EQ(res[2], GEN_CMD_STRING("-d"));
    ASSERT_EQ(res[3], GEN_CMD_STRING("\n"));
    ASSERT_EQ(res[4], GEN_CMD_STRING("-ns"));
  }
  argc = 0;
  argv = sdssplitargslong(line2.c_str(), &argc);
  ASSERT_TRUE(argv && argc == 5);
  if (argv) {
    ASSERT_STREQ(argv[0], "-f");
    ASSERT_STREQ(argv[1],
                 "/home/nico/.config/chromium/Default/Local Extension "
                 "Settings/ncdlagniojmheiklojdcpdaeepochckl/000100.ldb");
    ASSERT_STREQ(argv[2], "-d");
    ASSERT_STREQ(argv[3], "\n");
    ASSERT_STREQ(argv[4], "-ns");
    sds revert = sdsjoinsdsstable(argv, argc);
    ASSERT_STREQ(line2.c_str(), revert);
    sdsfree(revert);
    sdsfreesplitres(argv, argc);
  }

  const std::string line3 = "GET {NAMESPACE}:{TEST}";
  argc = 0;
  argv = sdssplitargs(line3.c_str(), &argc);
  ASSERT_TRUE(argv && argc == 2);
  if (argv) {
    ASSERT_STREQ(argv[0], "GET");
    ASSERT_STREQ(argv[1], "{NAMESPACE}:{TEST}");
  }

  const std::string line4 =
      R"(HMSET gameConfig:1:1 tile "note3" RY "1920" RX "1080" id 1)";
  {
    const common::char_buffer_t lineb = common::ConvertToCharBytes(line4);
    fastonosql::core::commands_args_t res;
    bool is_ok = fastonosql::core::ParseCommandLine(lineb, &res);
    ASSERT_TRUE(is_ok);
    ASSERT_EQ(res.size(), 10);
    ASSERT_EQ(res[0], GEN_CMD_STRING("HMSET"));
    ASSERT_EQ(res[1], GEN_CMD_STRING("gameConfig:1:1"));
    ASSERT_EQ(res[2], GEN_CMD_STRING("tile"));
    ASSERT_EQ(res[3], GEN_CMD_STRING("note3"));
    ASSERT_EQ(res[4], GEN_CMD_STRING("RY"));
    ASSERT_EQ(res[5], GEN_CMD_STRING("1920"));
    ASSERT_EQ(res[6], GEN_CMD_STRING("RX"));
    ASSERT_EQ(res[7], GEN_CMD_STRING("1080"));
    ASSERT_EQ(res[8], GEN_CMD_STRING("id"));
    ASSERT_EQ(res[9], GEN_CMD_STRING("1"));
  }

  argc = 0;
  argv = sdssplitargs(line4.c_str(), &argc);
  ASSERT_TRUE(argv && argc == 10);
  if (argv) {
    ASSERT_STREQ(argv[0], "HMSET");
    ASSERT_STREQ(argv[1], "gameConfig:1:1");
    ASSERT_STREQ(argv[2], "tile");
    ASSERT_STREQ(argv[3], "note3");
    ASSERT_STREQ(argv[4], "RY");
    ASSERT_STREQ(argv[5], "1920");
    ASSERT_STREQ(argv[6], "RX");
    ASSERT_STREQ(argv[7], "1080");
    ASSERT_STREQ(argv[8], "id");
    ASSERT_STREQ(argv[9], "1");
  }

  const std::string line5 = R"( SET    alex '{"test" : 1}')";
  {
    const common::char_buffer_t lineb = common::ConvertToCharBytes(line5);
    fastonosql::core::commands_args_t res;
    bool is_ok = fastonosql::core::ParseCommandLine(lineb, &res);
    ASSERT_TRUE(is_ok);
    ASSERT_EQ(res.size(), 3);
    ASSERT_EQ(res[0], GEN_CMD_STRING("SET"));
    ASSERT_EQ(res[1], GEN_CMD_STRING("alex"));
    ASSERT_EQ(res[2], GEN_CMD_STRING("{\"test\" : 1}"));
  }

  argc = 0;
  argv = sdssplitargs(line5.c_str(), &argc);
  ASSERT_TRUE(argv && argc == 3);
  if (argv) {
    ASSERT_STREQ(argv[0], "SET");
    ASSERT_STREQ(argv[1], "alex");
    ASSERT_STREQ(argv[2], "{\"test\" : 1}");
  }

  const std::string line6 = R"(SET alex "\x11\x22\x33")";
  const char etalon[] = {0x11, 0x22, 0x33};
  {
    const common::char_buffer_t lineb = common::ConvertToCharBytes(line6);
    fastonosql::core::commands_args_t res;
    bool is_ok = fastonosql::core::ParseCommandLine(lineb, &res);
    ASSERT_TRUE(is_ok);
    ASSERT_EQ(res.size(), 3);
    ASSERT_EQ(res[0], GEN_CMD_STRING("SET"));
    ASSERT_EQ(res[1], GEN_CMD_STRING("alex"));
    ASSERT_EQ(res[2], GEN_CMD_STRING_SIZE(etalon, SIZEOFMASS(etalon)));
  }

  argc = 0;
  argv = sdssplitargs(line6.c_str(), &argc);
  ASSERT_TRUE(argv && argc == 3);
  if (argv) {
    ASSERT_STREQ(argv[0], "SET");
    ASSERT_STREQ(argv[1], "alex");
    std::string str(argv[2], strlen(argv[2]));
    std::string eta(etalon, SIZEOFMASS(etalon));
    ASSERT_EQ(str, eta);
  }
}
