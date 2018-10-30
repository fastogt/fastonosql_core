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

#include <fastonosql/core/db/ssdb/db_connection.h>

#include <map>
#include <string>
#include <vector>

#include <common/convert2string.h>

#include <SSDB.h>  // for Status, Client

#include <fastonosql/core/db/ssdb/command_translator.h>
#include <fastonosql/core/db/ssdb/database_info.h>
#include "core/db/ssdb/internal/commands_api.h"

namespace fastonosql {
namespace core {
namespace ssdb {
namespace {
const ConstantCommandsArray kCommands = {CommandHolder(GEN_CMD_STRING(DB_HELP_COMMAND),
                                                       "[command]",
                                                       "Return how to use command",
                                                       UNDEFINED_SINCE,
                                                       DB_HELP_COMMAND " " DB_GET_KEY_COMMAND,
                                                       0,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Help),
                                         CommandHolder(GEN_CMD_STRING(DB_INFO_COMMAND),
                                                       "[section]",
                                                       "Return information about the server.",
                                                       UNDEFINED_SINCE,
                                                       DB_INFO_COMMAND " STATS",
                                                       0,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Info),
                                         CommandHolder(GEN_CMD_STRING(DB_GET_CONFIG_COMMAND),
                                                       "<parameter>",
                                                       "Get the value of a configuration parameter",
                                                       UNDEFINED_SINCE,
                                                       DB_GET_CONFIG_COMMAND " databases",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::ConfigGet),
                                         CommandHolder(GEN_CMD_STRING(DB_SCAN_COMMAND),
                                                       "<cursor> [MATCH pattern] [COUNT count]",
                                                       "Incrementally iterate the keys space",
                                                       UNDEFINED_SINCE,
                                                       DB_SCAN_COMMAND " 0 MATCH * COUNT 10",
                                                       1,
                                                       4,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Scan),
                                         CommandHolder(GEN_CMD_STRING(DB_JSONDUMP_COMMAND),
                                                       "<cursor> PATH <absolute_path> [MATCH pattern] [COUNT count]",
                                                       "Dump DB into json file by path.",
                                                       UNDEFINED_SINCE,
                                                       DB_JSONDUMP_COMMAND " 0 PATH ~/dump.json MATCH * COUNT 10",
                                                       3,
                                                       4,
                                                       CommandInfo::Native,
                                                       &CommandsApi::JsonDump),
                                         CommandHolder(GEN_CMD_STRING("SCANSSDB"),
                                                       "<key_start> <key_end> <limit>",
                                                       "List keys in range (key_start, key_end].",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::ScanSSDB),
                                         CommandHolder(GEN_CMD_STRING(DB_KEYS_COMMAND),
                                                       "<key_start> <key_end> <limit>",
                                                       "Find all keys matching the given limits.",
                                                       UNDEFINED_SINCE,
                                                       DB_KEYS_COMMAND " a z 10",
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Keys),
                                         CommandHolder(GEN_CMD_STRING(DB_DBKCOUNT_COMMAND),
                                                       "-",
                                                       "Return the number of keys in the "
                                                       "selected database",
                                                       UNDEFINED_SINCE,
                                                       DB_DBKCOUNT_COMMAND,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::DBkcount),
                                         CommandHolder(GEN_CMD_STRING(DB_FLUSHDB_COMMAND),
                                                       "-",
                                                       "Remove all keys from the current database",
                                                       UNDEFINED_SINCE,
                                                       DB_FLUSHDB_COMMAND,
                                                       0,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::FlushDB),
                                         CommandHolder(GEN_CMD_STRING(DB_SELECTDB_COMMAND),
                                                       "<name>",
                                                       "Change the selected database for the "
                                                       "current connection",
                                                       UNDEFINED_SINCE,
                                                       DB_SELECTDB_COMMAND " 0",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Select),
                                         CommandHolder(GEN_CMD_STRING(DB_SET_KEY_COMMAND),
                                                       "<key> <value>",
                                                       "Set the value of a key.",
                                                       UNDEFINED_SINCE,
                                                       DB_SET_KEY_COMMAND " key value",
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Set),
                                         CommandHolder(GEN_CMD_STRING(DB_GET_KEY_COMMAND),
                                                       "<key>",
                                                       "Get the value of a key.",
                                                       UNDEFINED_SINCE,
                                                       DB_GET_KEY_COMMAND " key",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Get),
                                         CommandHolder(GEN_CMD_STRING(DB_RENAME_KEY_COMMAND),
                                                       "<key> <newkey>",
                                                       "Rename a key",
                                                       UNDEFINED_SINCE,
                                                       DB_RENAME_KEY_COMMAND " old_name new_name",
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Rename),
                                         CommandHolder(GEN_CMD_STRING(DB_DELETE_KEY_COMMAND),
                                                       "<key> [key ...]",
                                                       "Delete key.",
                                                       UNDEFINED_SINCE,
                                                       DB_DELETE_KEY_COMMAND " key",
                                                       1,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Delete),
                                         CommandHolder(GEN_CMD_STRING(DB_SET_TTL_COMMAND),
                                                       "<key> <exptime>",
                                                       "Set a key's time to live in seconds",
                                                       UNDEFINED_SINCE,
                                                       DB_SET_TTL_COMMAND " key 10",
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::SetTTL),
                                         CommandHolder(GEN_CMD_STRING(DB_GET_TTL_COMMAND),
                                                       "<key>",
                                                       "Get the time to live for a key",
                                                       UNDEFINED_SINCE,
                                                       DB_GET_TTL_COMMAND " key",
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::GetTTL),
                                         CommandHolder(GEN_CMD_STRING(DB_QUIT_COMMAND),
                                                       "-",
                                                       "Close the connection",
                                                       UNDEFINED_SINCE,
                                                       DB_QUIT_COMMAND,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Quit),
                                         CommandHolder(GEN_CMD_STRING("AUTH"),
                                                       "<password>",
                                                       "Authenticate to the server",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Auth),
                                         CommandHolder(GEN_CMD_STRING("SETX"),
                                                       "<key> <value> <ttl>",
                                                       "Set the value of the key, with a time to live.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Setx),
                                         CommandHolder(GEN_CMD_STRING("INCR"),
                                                       "<key> [num]",
                                                       "Increment the number stored at key by num.\n"
                                                       "The num argument could be a negative integer.\n"
                                                       "The old number is first converted to an integer "
                                                       "before "
                                                       "increment, assuming it "
                                                       "was stored as literal integer.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       1,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Incr),
                                         CommandHolder(GEN_CMD_STRING("RSCAN"),
                                                       "<key_start> <key_end> <limit>",
                                                       "List keys in range (key_start, key_end].",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Rscan),
                                         CommandHolder(GEN_CMD_STRING("MULTI_GET"),
                                                       "<keys> [keys ...]",
                                                       "Get the values related to the specified "
                                                       "multiple keys",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiGet),
                                         CommandHolder(GEN_CMD_STRING("MULTI_SET"),
                                                       "<key> <value> [key value ...]",
                                                       "Set multiple key-value pairs(kvs) in "
                                                       "one method call.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiSet),
                                         CommandHolder(GEN_CMD_STRING("MULTI_DEL"),
                                                       "<keys> [keys ...]",
                                                       "Delete specified multiple keys.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiDel),
                                         CommandHolder(GEN_CMD_STRING("HSET"),
                                                       "<name> <key> <value>",
                                                       "Set the string value in argument as "
                                                       "value of the key of a hashmap.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hset),
                                         CommandHolder(GEN_CMD_STRING("HGET"),
                                                       "<name> <key>",
                                                       "Get the value related to the specified "
                                                       "key of a hashmap",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hget),
                                         CommandHolder(GEN_CMD_STRING("HDEL"),
                                                       "<name> <key>",
                                                       "Delete specified key of a hashmap.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hdel),
                                         CommandHolder(GEN_CMD_STRING("HINCR"),
                                                       "<name> <key> <num>",
                                                       "Increment the number stored at key in a hashmap "
                                                       "by num.\n"
                                                       "The num argument could be a negative integer.\n"
                                                       "The old number is first converted to an integer "
                                                       "before "
                                                       "increment, assuming it "
                                                       "was stored as literal integer.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hincr),
                                         CommandHolder(GEN_CMD_STRING("HSIZE"),
                                                       "<name>",
                                                       "Return the number of pairs of a hashmap.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hsize),
                                         CommandHolder(GEN_CMD_STRING("HCLEAR"),
                                                       "<name>",
                                                       "Delete all keys in a hashmap.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hclear),
                                         CommandHolder(GEN_CMD_STRING("HKEYS"),
                                                       "<name> <key_start> <key_end> <limit>",
                                                       "List keys of a hashmap in range "
                                                       "(key_start, key_end].",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hkeys),
                                         CommandHolder(GEN_CMD_STRING("HGETALL"),
                                                       "<name>",
                                                       "Returns the whole hash, as an array of "
                                                       "strings indexed by strings.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hgetall),
                                         CommandHolder(GEN_CMD_STRING("HSCAN"),
                                                       "<name> <key_start> <key_end> <limit>",
                                                       "List key-value pairs of a hashmap with "
                                                       "keys in range "
                                                       "(key_start, key_end].",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hscan),
                                         CommandHolder(GEN_CMD_STRING("HRSCAN"),
                                                       "<name> <key_start> <key_end> <limit>",
                                                       "List key-value pairs of a hashmap with "
                                                       "keys in range "
                                                       "(key_start, key_end], in "
                                                       "reverse order.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Hrscan),
                                         CommandHolder(GEN_CMD_STRING("MULTI_HGET"),
                                                       "<name> <keys>",
                                                       "Get the values related to the specified "
                                                       "multiple keys of a hashmap.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiHget),
                                         CommandHolder(GEN_CMD_STRING("MULTI_HSET"),
                                                       "<name> <key> <value> [key value ...]",
                                                       "Set multiple key-value pairs(kvs) of a "
                                                       "hashmap in one method call.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiHset),
                                         CommandHolder(GEN_CMD_STRING("ZSET"),
                                                       "<name> <key> <score>",
                                                       "Set the score of the key of a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zset),
                                         CommandHolder(GEN_CMD_STRING("ZGET"),
                                                       "<name> <key>",
                                                       "Get the score related to the specified "
                                                       "key of a zset",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zget),
                                         CommandHolder(GEN_CMD_STRING("ZDEL"),
                                                       "<name> <key>",
                                                       "Delete specified key of a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zdel),
                                         CommandHolder(GEN_CMD_STRING("ZINCR"),
                                                       "<name> <key> <num>",
                                                       "Increment the number stored at key in a zset by "
                                                       "num.\n"
                                                       "The num argument could be a negative integer.\n"
                                                       "The old number is first converted to an integer "
                                                       "before "
                                                       "increment, assuming it "
                                                       "was stored as literal integer.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zincr),
                                         CommandHolder(GEN_CMD_STRING("ZSIZE"),
                                                       "<name>",
                                                       "Return the number of pairs of a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zsize),
                                         CommandHolder(GEN_CMD_STRING("ZCLEAR"),
                                                       "<name>",
                                                       "Delete all keys in a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zclear),
                                         CommandHolder(GEN_CMD_STRING("ZRANK"),
                                                       "<name> <key>",
                                                       "Returns the rank(index) of a given key in the "
                                                       "specified "
                                                       "sorted set, starting at "
                                                       "0 for the item with the smallest score.\n"
                                                       "zrrank starts at 0 for the item with the largest "
                                                       "score.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zrank),
                                         CommandHolder(GEN_CMD_STRING("ZRRANK"),
                                                       "<name> <key>",
                                                       "Returns the rank(index) of a given key in the "
                                                       "specified "
                                                       "sorted set, starting at "
                                                       "0 for the item with the smallest score.\n"
                                                       "zrrank starts at 0 for the item with the largest "
                                                       "score.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zrrank),
                                         CommandHolder(GEN_CMD_STRING("ZRANGE"),
                                                       "<name> <offset> <limit>",
                                                       "Returns a range of key-score pairs by "
                                                       "index range [offset, "
                                                       "offset + limit). "
                                                       "Zrrange iterates in reverse order.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zrange),
                                         CommandHolder(GEN_CMD_STRING("ZRRANGE"),
                                                       "<name> <offset> <limit>",
                                                       "Returns a range of key-score pairs by "
                                                       "index range [offset, "
                                                       "offset + limit). "
                                                       "Zrrange iterates in reverse order.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zrrange),
                                         CommandHolder(GEN_CMD_STRING("ZKEYS"),
                                                       "<name> <key_start> <score_start> "
                                                       "<score_end> <limit>",
                                                       "List keys in a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       4,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zkeys),
                                         CommandHolder(GEN_CMD_STRING("ZSCAN"),
                                                       "<name> <key_start> <score_start> "
                                                       "<score_end> <limit>",
                                                       "List key-score pairs in a zset, where "
                                                       "key-score in range "
                                                       "(key_start+score_start, "
                                                       "score_end].\n"
                                                       "If key_start is empty, keys with a "
                                                       "score greater than or equal to "
                                                       "score_start will be "
                                                       "returned.\n"
                                                       "If key_start is not empty, keys with "
                                                       "score larger than score_start, "
                                                       "and keys larger than key_start also "
                                                       "with score equal to score_start "
                                                       "will be returned.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       5,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zscan),
                                         CommandHolder(GEN_CMD_STRING("ZRSCAN"),
                                                       "<name> <key_start> <score_start> <score_end> "
                                                       "<limit>",
                                                       "List key-score pairs of a zset, in reverse order.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       5,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Zrscan),
                                         CommandHolder(GEN_CMD_STRING("MULTI_ZGET"),
                                                       "<name> <keys>",
                                                       "Get the values related to the specified "
                                                       "multiple keys of a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiZget),
                                         CommandHolder(GEN_CMD_STRING("MULTI_ZSET"),
                                                       "<name> <key> <score> [key score ...]",
                                                       "Set multiple key-score pairs(kvs) of a "
                                                       "zset in one method call.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       INFINITE_COMMAND_ARGS,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiZset),
                                         CommandHolder(GEN_CMD_STRING("MULTI_ZDEL"),
                                                       "<name> <keys>",
                                                       "Delete specified multiple keys of a zset.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::MultiZdel),
                                         CommandHolder(GEN_CMD_STRING("QPUSH"),
                                                       "<name> <item>",
                                                       "Adds an or more than one element to the "
                                                       "end of the queue.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Qpush),
                                         CommandHolder(GEN_CMD_STRING("QPOP"),
                                                       "<name> <size>",
                                                       "Pop out one or more elements from the "
                                                       "head of a queue.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       2,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Qpop),
                                         CommandHolder(GEN_CMD_STRING("QSLICE"),
                                                       "<name> <begin> <end>",
                                                       "Returns a portion of elements from the "
                                                       "queue at the "
                                                       "specified range [begin, "
                                                       "end]. begin and end could be negative.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       3,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Qslice),
                                         CommandHolder(GEN_CMD_STRING("QCLEAR"),
                                                       "<name>",
                                                       "Clear the queue.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       1,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::Qclear),
                                         CommandHolder(GEN_CMD_STRING("DBSIZE"),
                                                       "-",
                                                       "Return the approximate size of the database, in "
                                                       "bytes. If "
                                                       "compression is "
                                                       "enabled, the size will be of the compressed data.",
                                                       UNDEFINED_SINCE,
                                                       UNDEFINED_EXAMPLE_STR,
                                                       0,
                                                       0,
                                                       CommandInfo::Native,
                                                       &CommandsApi::DBsize)};
}  // namespace
}  // namespace ssdb
template <>
const char* ConnectionTraits<SSDB>::GetBasedOn() {
  return "ssdb";
}

template <>
const char* ConnectionTraits<SSDB>::GetVersionApi() {
  return "1.9.4";
}

template <>
const ConstantCommandsArray& ConnectionCommandsTraits<SSDB>::GetCommands() {
  return ssdb::kCommands;
}
namespace internal {
template <>
common::Error ConnectionAllocatorTraits<ssdb::NativeConnection, ssdb::Config>::Connect(const ssdb::Config& config,
                                                                                       ssdb::NativeConnection** hout) {
  ssdb::NativeConnection* context = nullptr;
  common::Error err = ssdb::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  return common::Error();
}

template <>
common::Error ConnectionAllocatorTraits<ssdb::NativeConnection, ssdb::Config>::Disconnect(
    ssdb::NativeConnection** handle) {
  destroy(handle);
  return common::Error();
}

template <>
bool ConnectionAllocatorTraits<ssdb::NativeConnection, ssdb::Config>::IsConnected(ssdb::NativeConnection* handle) {
  if (!handle) {
    return false;
  }

  return true;
}

}  // namespace internal
namespace ssdb {
namespace {
common::Error AuthContext(::ssdb::Client* context, const std::string& password) {
  if (!context) {
    return common::make_error_inval();
  }

  if (password.empty()) {  // handle in checkresult
    return common::Error();
  }

  auto st = context->auth(password);
  if (st.error()) {
    return common::make_error("AUTH function error: need authentification!");
  }
  return common::Error();
}
}  // namespace
common::Error CreateConnection(const Config& config, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  DCHECK(*context == nullptr);
  ::ssdb::Client* lcontext = ::ssdb::Client::connect(config.host.GetHost(), config.host.GetPort());
  if (!lcontext) {
    return common::make_error("Fail connect to server!");
  }

  *context = lcontext;
  return common::Error();
}

common::Error TestConnection(const Config& config) {
  ::ssdb::Client* ssdb = nullptr;
  common::Error err = CreateConnection(config, &ssdb);
  if (err) {
    return err;
  }

  err = AuthContext(ssdb, config.auth);
  if (err) {
    delete ssdb;
    return err;
  }

  delete ssdb;
  return common::Error();
}

DBConnection::DBConnection(CDBConnectionClient* client)
    : base_class(client, new CommandTranslator(base_class::GetCommands())), is_auth_(false) {}

bool DBConnection::IsAuthenticated() const {
  if (!base_class::IsAuthenticated()) {
    return false;
  }

  return is_auth_;
}

common::Error DBConnection::Connect(const config_t& config) {
  common::Error err = base_class::Connect(config);
  if (err) {
    return err;
  }

  err = Auth(config->auth);
  if (err) {
    return err;
  }

  return common::Error();
}

common::Error DBConnection::Disconnect() {
  is_auth_ = false;
  return base_class::Disconnect();
}

common::Error DBConnection::Info(const command_buffer_t& args, ServerInfo::Stats* statsout) {
  if (!statsout) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> ret;
  err = CheckResultCommand(DB_INFO_COMMAND, connection_.handle_->info(common::ConvertToString(args), &ret));
  if (err) {
    return err;
  }

  ServerInfo::Stats lstatsout;
  for (size_t i = 0; i < ret.size(); i += 2) {
    if (ret[i] == SSDB_COMMON_VERSION_LABEL) {
      lstatsout.version = ret[i + 1];
    } else if (ret[i] == SSDB_COMMON_LINKS_LABEL) {
      uint32_t links;
      if (common::ConvertFromString(ret[i + 1], &links)) {
        lstatsout.links = links;
      }
    } else if (ret[i] == SSDB_COMMON_TOTAL_CALLS_LABEL) {
      uint32_t total_calls;
      if (common::ConvertFromString(ret[i + 1], &total_calls)) {
        lstatsout.total_calls = total_calls;
      }
    } else if (ret[i] == SSDB_COMMON_DBSIZE_LABEL) {
      uint32_t dbsize;
      if (common::ConvertFromString(ret[i + 1], &dbsize)) {
        lstatsout.dbsize = dbsize;
      }
    } else if (ret[i] == SSDB_COMMON_BINLOGS_LABEL) {
      lstatsout.binlogs = ret[i + 1];
    }
  }

  *statsout = lstatsout;
  return common::Error();
}

common::Error DBConnection::DBsize(int64_t* size) {
  if (!size) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  int64_t ret;
  err = CheckResultCommand("DBSIZE", connection_.handle_->dbsize(&ret));
  if (err) {
    return err;
  }

  *size = ret;
  return common::Error();
}

common::Error DBConnection::Auth(const std::string& password) {
  common::Error err = TestIsConnected();
  if (err) {
    return err;
  }

  err = AuthContext(connection_.handle_, password);
  if (err) {
    is_auth_ = false;
    return err;
  }

  is_auth_ = true;
  return common::Error();
}

common::Error DBConnection::Setx(const raw_key_t& key, const raw_value_t& value, ttl_t ttl) {
  if (key.empty() || value.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("SETX", connection_.handle_->setx(key.data(), value.data(), static_cast<int>(ttl)));
}

common::Error DBConnection::SetInner(const raw_key_t& key, const raw_value_t& value) {
  const std::string key_slice = common::ConvertToString(key);
  const std::string value_str = common::ConvertToString(value);
  return CheckResultCommand(DB_SET_KEY_COMMAND, connection_.handle_->set(key_slice, value_str));
}

common::Error DBConnection::GetInner(const raw_key_t& key, raw_value_t* ret_val) {
  const std::string key_slice = common::ConvertToString(key);
  std::string ret_str;
  common::Error err = CheckResultCommand(DB_GET_KEY_COMMAND, connection_.handle_->get(key_slice, &ret_str));
  if (err) {
    return err;
  }

  *ret_val = common::ConvertToCharBytes(ret_str);
  return common::Error();
}

common::Error DBConnection::DelInner(const raw_key_t& key) {
  const std::string key_slice = common::ConvertToString(key);
  return CheckResultCommand(DB_DELETE_KEY_COMMAND, connection_.handle_->del(key_slice));
}

common::Error DBConnection::Incr(const raw_key_t& key, int64_t incrby, int64_t* out) {
  if (key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("INCR", connection_.handle_->incr(key.data(), incrby, out));
}

common::Error DBConnection::ScanSsdb(const raw_key_t& key_start,
                                     const raw_key_t& key_end,
                                     uint64_t limit,
                                     raw_keys_t* out) {
  if (!out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> keys;
  err = CheckResultCommand(DB_SCAN_COMMAND, connection_.handle_->scan(key_start.data(), key_end.data(), limit, &keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(keys[i]));
  }
  return common::Error();
}

common::Error DBConnection::Rscan(const raw_key_t& key_start,
                                  const raw_key_t& key_end,
                                  uint64_t limit,
                                  raw_keys_t* out) {
  if (!out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> keys;
  err = CheckResultCommand("RSCAN", connection_.handle_->rscan(key_start.data(), key_end.data(), limit, &keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(keys[i]));
  }
  return common::Error();
}

common::Error DBConnection::MultiGet(const raw_keys_t& keys, raw_values_t* out) {
  if (keys.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> stabled_keys;
  for (size_t i = 0; i < keys.size(); ++i) {
    stabled_keys.push_back(common::ConvertToString(keys[i]));
  }

  std::vector<std::string> values;
  err = CheckResultCommand("MULTIGET", connection_.handle_->multi_get(stabled_keys, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::MultiSet(const std::map<raw_key_t, raw_value_t>& kvs) {
  if (kvs.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::map<std::string, std::string> stabled_kvs;
  for (auto it = kvs.begin(); it != kvs.end(); ++it) {
    stabled_kvs[common::ConvertToString(it->first)] = common::ConvertToString(it->second);
  }

  return CheckResultCommand("MULTISET", connection_.handle_->multi_set(stabled_kvs));
}

common::Error DBConnection::MultiDel(const raw_keys_t& keys) {
  if (keys.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> stabled_keys;
  for (size_t i = 0; i < keys.size(); ++i) {
    stabled_keys.push_back(common::ConvertToString(keys[i]));
  }
  return CheckResultCommand("MULTIDEL", connection_.handle_->multi_del(stabled_keys));
}

common::Error DBConnection::Hget(const raw_key_t& name, const raw_key_t& key, raw_value_t* out) {
  if (name.empty() || key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::string res;
  err = CheckResultCommand("HGET", connection_.handle_->hget(name.data(), key.data(), &res));
  if (err) {
    return err;
  }

  *out = common::ConvertToCharBytes(res);
  return common::Error();
}

common::Error DBConnection::Hgetall(const raw_key_t& name, raw_keys_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> keys;
  err = CheckResultCommand("HGETALL", connection_.handle_->hgetall(name.data(), &keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(keys[i]));
  }
  return common::Error();
}

common::Error DBConnection::Hset(const raw_key_t& name, const raw_key_t& key, const raw_key_t& val) {
  if (name.empty() || key.empty() || val.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("HSET", connection_.handle_->hset(name.data(), key.data(), val.data()));
}

common::Error DBConnection::Hdel(const raw_key_t& name, const raw_key_t& key) {
  if (name.empty() || key.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("HDEL", connection_.handle_->hdel(name.data(), key.data()));
}

common::Error DBConnection::Hincr(const raw_key_t& name, const raw_key_t& key, int64_t incrby, int64_t* out) {
  if (name.empty() || key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("HINCR", connection_.handle_->hincr(name.data(), key.data(), incrby, out));
}

common::Error DBConnection::Hsize(const raw_key_t& name, int64_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("HSIZE", connection_.handle_->hsize(name.data(), out));
}

common::Error DBConnection::Hclear(const raw_key_t& name, int64_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("HCLEAR", connection_.handle_->hclear(name.data(), out));
}

common::Error DBConnection::Hkeys(const raw_key_t& name,
                                  const raw_key_t& key_start,
                                  const raw_key_t& key_end,
                                  uint64_t limit,
                                  raw_keys_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> keys;
  err = CheckResultCommand("HKEYS",
                           connection_.handle_->hkeys(name.data(), key_start.data(), key_end.data(), limit, &keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(keys[i]));
  }
  return common::Error();
}

common::Error DBConnection::Hscan(const raw_key_t& name,
                                  const raw_key_t& key_start,
                                  const raw_key_t& key_end,
                                  uint64_t limit,
                                  raw_keys_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> keys;
  err = CheckResultCommand("HSCAN",
                           connection_.handle_->hscan(name.data(), key_start.data(), key_end.data(), limit, &keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(keys[i]));
  }
  return common::Error();
}

common::Error DBConnection::Hrscan(const raw_key_t& name,
                                   const raw_key_t& key_start,
                                   const raw_key_t& key_end,
                                   uint64_t limit,
                                   raw_keys_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> keys;
  err = CheckResultCommand("HRSCAN",
                           connection_.handle_->hrscan(name.data(), key_start.data(), key_end.data(), limit, &keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < keys.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(keys[i]));
  }
  return common::Error();
}

common::Error DBConnection::MultiHget(const raw_key_t& name, const raw_keys_t& keys, raw_values_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> skeys;
  for (size_t i = 0; i < keys.size(); ++i) {
    skeys.push_back(common::ConvertToString(keys[i]));
  }

  std::vector<std::string> values;
  err = CheckResultCommand("MULTIHGET", connection_.handle_->multi_hget(name.data(), skeys, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::MultiHset(const raw_key_t& name, const std::map<raw_key_t, raw_value_t>& keys) {
  if (name.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::map<std::string, std::string> skeys;
  for (auto it = keys.begin(); it != keys.end(); ++it) {
    skeys[common::ConvertToString(it->first)] = common::ConvertToString(it->second);
  }
  return CheckResultCommand("MULTIHSET", connection_.handle_->multi_hset(name.data(), skeys));
}

common::Error DBConnection::Zget(const raw_key_t& name, const raw_key_t& key, int64_t* out) {
  if (name.empty() || key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZGET", connection_.handle_->zget(name.data(), key.data(), out));
}

common::Error DBConnection::Zset(const raw_key_t& name, const raw_key_t& key, int64_t score) {
  if (name.empty() || key.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZSET", connection_.handle_->zset(name.data(), key.data(), score));
}

common::Error DBConnection::Zdel(const raw_key_t& name, const raw_key_t& key) {
  if (name.empty() || key.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZDEL", connection_.handle_->zdel(name.data(), key.data()));
}

common::Error DBConnection::Zincr(const raw_key_t& name, const raw_key_t& key, int64_t incrby, int64_t* out) {
  if (name.empty() || key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZINCR", connection_.handle_->zincr(name.data(), key.data(), incrby, out));
}

common::Error DBConnection::Zsize(const raw_key_t& name, int64_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZSIZE", connection_.handle_->zsize(name.data(), out));
}

common::Error DBConnection::Zclear(const raw_key_t& name, int64_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZCLEAR", connection_.handle_->zclear(name.data(), out));
}

common::Error DBConnection::Zrank(const raw_key_t& name, const raw_key_t& key, int64_t* out) {
  if (name.empty() || key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZRANK", connection_.handle_->zrank(name.data(), key.data(), out));
}

common::Error DBConnection::Zrrank(const raw_key_t& name, const raw_key_t& key, int64_t* out) {
  if (name.empty() || key.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("ZRRANK", connection_.handle_->zrrank(name.data(), key.data(), out));
}

common::Error DBConnection::Zrange(const raw_key_t& name, uint64_t offset, uint64_t limit, raw_values_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> values;
  err = CheckResultCommand("ZRANGE", connection_.handle_->zrange(name.data(), offset, limit, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::Zrrange(const raw_key_t& name, uint64_t offset, uint64_t limit, raw_values_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> values;
  err = CheckResultCommand("ZRRANGE", connection_.handle_->zrrange(name.data(), offset, limit, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::Zkeys(const raw_key_t& name,
                                  const raw_key_t& key_start,
                                  int64_t* score_start,
                                  int64_t* score_end,
                                  uint64_t limit,
                                  raw_values_t* out) {
  if (name.empty() || !score_start || !score_end || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> values;
  err = CheckResultCommand(
      "ZKEYS", connection_.handle_->zkeys(name.data(), key_start.data(), score_start, score_end, limit, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::Zscan(const raw_key_t& name,
                                  const raw_key_t& key_start,
                                  int64_t* score_start,
                                  int64_t* score_end,
                                  uint64_t limit,
                                  raw_values_t* out) {
  if (name.empty() || !score_start || !score_end || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> values;
  err = CheckResultCommand(
      "ZSCAN", connection_.handle_->zscan(name.data(), key_start.data(), score_start, score_end, limit, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::Zrscan(const raw_key_t& name,
                                   const raw_key_t& key_start,
                                   int64_t* score_start,
                                   int64_t* score_end,
                                   uint64_t limit,
                                   raw_values_t* out) {
  if (name.empty() || !score_start || !score_end || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> values;
  err = CheckResultCommand(
      "ZRSCAN", connection_.handle_->zrscan(name.data(), key_start.data(), score_start, score_end, limit, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::MultiZget(const raw_key_t& name, const raw_keys_t& keys, raw_values_t* out) {
  if (name.empty() || keys.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> skeys;
  for (size_t i = 0; i < keys.size(); ++i) {
    skeys.push_back(common::ConvertToString(keys[i]));
  }

  std::vector<std::string> values;
  err = CheckResultCommand("MULTIZGET", connection_.handle_->multi_zget(name.data(), skeys, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::MultiZset(const raw_key_t& name, const std::map<raw_key_t, int64_t>& kss) {
  if (name.empty() || kss.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::map<std::string, int64_t> stabled_kvs;
  for (auto it = kss.begin(); it != kss.end(); ++it) {
    stabled_kvs[common::ConvertToString(it->first)] = it->second;
  }
  return CheckResultCommand("MULTIZSET", connection_.handle_->multi_zset(name.data(), stabled_kvs));
}

common::Error DBConnection::MultiZdel(const raw_key_t& name, const raw_keys_t& keys) {
  if (name.empty() || keys.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> skeys;
  for (size_t i = 0; i < keys.size(); ++i) {
    skeys.push_back(common::ConvertToString(keys[i]));
  }
  return CheckResultCommand("MULTIZDEL", connection_.handle_->multi_zdel(name.data(), skeys));
}

common::Error DBConnection::Qpush(const raw_key_t& name, const raw_key_t& item) {
  if (name.empty() || item.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("QPUSH", connection_.handle_->qpush(name.data(), item.data()));
}

common::Error DBConnection::Qpop(const raw_key_t& name, raw_value_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::string value;
  err = CheckResultCommand("QPOP", connection_.handle_->qpop(name.data(), &value));
  if (err) {
    return err;
  }

  *out = common::ConvertToCharBytes(value);
  return common::Error();
}

common::Error DBConnection::Qslice(const raw_key_t& name, int64_t begin, int64_t end, raw_values_t* out) {
  if (name.empty() || !out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  std::vector<std::string> values;
  err = CheckResultCommand("QSLICE", connection_.handle_->qslice(name.data(), begin, end, &values));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < values.size(); ++i) {
    out->push_back(common::ConvertToCharBytes(values[i]));
  }
  return common::Error();
}

common::Error DBConnection::Qclear(const raw_key_t& name, int64_t* ret) {
  if (name.empty() || !ret) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  return CheckResultCommand("QCLEAR", connection_.handle_->qclear(name.data(), ret));
}

common::Error DBConnection::Expire(key_t key, ttl_t ttl) {
  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  const std::string key_slice = common::ConvertToString(key.GetData());
  return CheckResultCommand(DB_SET_TTL_COMMAND, connection_.handle_->expire(key_slice, static_cast<int>(ttl)));
}

common::Error DBConnection::TTL(key_t key, ttl_t* ttl) {
  if (!ttl) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = TestIsAuthenticated();
  if (err) {
    return err;
  }

  int lttl = 0;
  const std::string key_slice = common::ConvertToString(key.GetData());
  err = CheckResultCommand(DB_GET_TTL_COMMAND, connection_.handle_->ttl(key_slice, &lttl));
  if (err) {
    return err;
  }
  *ttl = lttl;
  return common::Error();
}

common::Error DBConnection::ConfigGetDatabasesImpl(db_names_t* dbs) {
  db_names_t ldbs = {GetCurrentDBName()};
  *dbs = ldbs;
  return common::Error();
}

common::Error DBConnection::ScanImpl(cursor_t cursor_in,
                                     const pattern_t& pattern,
                                     keys_limit_t count_keys,
                                     raw_keys_t* keys_out,
                                     cursor_t* cursor_out) {
  std::vector<std::string> ret;
  common::Error err =
      CheckResultCommand(DB_SCAN_COMMAND, connection_.handle_->keys(std::string(), std::string(), count_keys, &ret));
  if (err) {
    return err;
  }

  cursor_t offset_pos = cursor_in;
  cursor_t lcursor_out = 0;
  raw_keys_t lkeys_out;
  for (size_t i = 0; i < ret.size(); ++i) {
    command_buffer_t key = common::ConvertToCharBytes(ret[i]);
    if (lkeys_out.size() < count_keys) {
      if (common::MatchPattern(key, pattern)) {
        if (offset_pos == 0) {
          lkeys_out.push_back(key);
        } else {
          offset_pos--;
        }
      }
    } else {
      lcursor_out = cursor_in + count_keys;
      break;
    }
  }

  *keys_out = lkeys_out;
  *cursor_out = lcursor_out;
  return common::Error();
}

common::Error DBConnection::KeysImpl(const command_buffer_t& key_start,
                                     const command_buffer_t& key_end,
                                     keys_limit_t limit,
                                     raw_keys_t* ret) {
  std::vector<std::string> ret_keys;
  const std::string kstart = common::ConvertToString(key_start);
  const std::string kend = common::ConvertToString(key_end);
  common::Error err = CheckResultCommand(DB_KEYS_COMMAND, connection_.handle_->keys(kstart, kend, limit, &ret_keys));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < ret_keys.size(); ++i) {
    std::string key = ret_keys[i];
    (*ret).push_back(common::ConvertToCharBytes(key));
  }
  return common::Error();
}

common::Error DBConnection::DBkcountImpl(keys_limit_t* size) {
  std::vector<std::string> ret;
  common::Error err =
      CheckResultCommand(DB_DBKCOUNT_COMMAND,
                         connection_.handle_->keys(std::string(), std::string(), UINT64_MAX, &ret));  // may be dbsize?
  if (err) {
    return err;
  }

  *size = ret.size();
  return common::Error();
}

common::Error DBConnection::FlushDBImpl() {
  std::vector<std::string> ret;
  common::Error err =
      CheckResultCommand(DB_FLUSHDB_COMMAND, connection_.handle_->keys(std::string(), std::string(), 0, &ret));
  if (err) {
    return err;
  }

  for (size_t i = 0; i < ret.size(); ++i) {
    key_t key(ret[i]);
    common::Error err = DelInner(key.GetData());
    if (err) {
      return err;
    }
  }

  return common::Error();
}

common::Error DBConnection::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  if (name != GetCurrentDBName()) {
    return ICommandTranslator::InvalidInputArguments(GEN_CMD_STRING(DB_SELECTDB_COMMAND));
  }

  keys_limit_t kcount = 0;
  common::Error err = DBkcount(&kcount);
  DCHECK(!err) << err->GetDescription();
  *info = new DataBaseInfo(name, true, kcount);
  return common::Error();
}

common::Error DBConnection::DeleteImpl(const NKeys& keys, NKeys* deleted_keys) {
  for (size_t i = 0; i < keys.size(); ++i) {
    NKey key = keys[i];
    key_t key_str = key.GetKey();
    common::Error err = DelInner(key_str.GetData());
    if (err) {
      continue;
    }

    deleted_keys->push_back(key);
  }

  return common::Error();
}

common::Error DBConnection::RenameImpl(const NKey& key, const key_t& new_key) {
  const key_t key_str = key.GetKey();
  const raw_key_t rkey = key_str.GetData();

  raw_value_t value_str;
  common::Error err = GetInner(rkey, &value_str);
  if (err) {
    return err;
  }

  err = DelInner(rkey);
  if (err) {
    return err;
  }

  const raw_key_t nkey = new_key.GetData();
  return SetInner(nkey, value_str);
}

common::Error DBConnection::SetImpl(const NDbKValue& key, NDbKValue* added_key) {
  const NKey cur = key.GetKey();
  const auto key_str = cur.GetKey();
  const NValue value = key.GetValue();
  const auto value_str = value.GetReadableValue();

  common::Error err = SetInner(key_str.GetData(), value_str.GetData());
  if (err) {
    return err;
  }

  *added_key = key;
  return common::Error();
}

common::Error DBConnection::GetImpl(const NKey& key, NDbKValue* loaded_key) {
  const key_t key_str = key.GetKey();

  raw_value_t value_str;
  common::Error err = GetInner(key_str.GetData(), &value_str);
  if (err) {
    return err;
  }

  NValue val(common::Value::CreateStringValue(value_str));
  *loaded_key = NDbKValue(key, val);
  return common::Error();
}

common::Error DBConnection::SetTTLImpl(const NKey& key, ttl_t ttl) {
  const key_t key_str = key.GetKey();
  return Expire(key_str, ttl);
}

common::Error DBConnection::GetTTLImpl(const NKey& key, ttl_t* ttl) {
  const key_t key_str = key.GetKey();
  return TTL(key_str, ttl);
}

common::Error DBConnection::QuitImpl() {
  return Disconnect();
}

common::Error DBConnection::CheckResultCommand(const std::string& cmd, const ::ssdb::Status& err) {
  if (err.error()) {
    if (err.code() == "noauth") {
      is_auth_ = false;
    }
    return GenerateError(cmd, err.code());
  }

  return common::Error();
}

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
