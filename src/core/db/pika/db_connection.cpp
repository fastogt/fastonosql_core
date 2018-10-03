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

#include <fastonosql/core/db/pika/db_connection.h>

#include <hiredis/hiredis.h>

#include <fastonosql/core/constant_commands_array.h>
#include <fastonosql/core/db/pika/internal/commands_api.h>
#include <fastonosql/core/db/pika/server_info.h>

#define DBSIZE "INFO KEYSPACE"

namespace fastonosql {
namespace core {
namespace pika {
namespace {
const ConstantCommandsArray g_commands = {
    CommandHolder(DB_HELP_COMMAND, "[command]", "Return how to use command",
                  UNDEFINED_SINCE, DB_HELP_COMMAND " " DB_GET_KEY_COMMAND, 0, 1,
                  CommandInfo::Native, &CommandsApi::Help),
    CommandHolder(DB_INFO_COMMAND, "[section]",
                  "Get information and statistics about the server",
                  UNDEFINED_SINCE, DB_INFO_COMMAND " STATS", 0, 2,
                  CommandInfo::Native, &CommandsApi::Info),
    CommandHolder("APPEND", "<key> <value>", "Append a value to a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Append),
    CommandHolder("AUTH", "<password>", "Authenticate to the server",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Auth),
    CommandHolder("BGREWRITEAOF", "-",
                  "Asynchronously rewrite the append-only file",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::BgRewriteAof),
    CommandHolder("BGSAVE", "-", "Asynchronously save the dataset to disk",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::BgSave),
    CommandHolder("BITCOUNT", "<key> [start] [end]",
                  "Count set bits in a string", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 2, CommandInfo::Native,
                  &CommandsApi::BitCount),
    CommandHolder("BITFIELD",
                  "key [GET type offset] [SET type offset value] [INCRBY type "
                  "offset increment] "
                  "[OVERFLOW WRAP|SAT|FAIL]",
                  "Perform arbitrary bitfield integer operations on strings",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 8,
                  CommandInfo::Native, &CommandsApi::BitField),
    CommandHolder("BITOP", "<operation> <destkey> <key> [key ...]",
                  "Perform bitwise operations between strings", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 2, CommandInfo::Native,
                  &CommandsApi::BitOp),
    CommandHolder("BITPOS", "<key> <bit> [start] [end]",
                  "Find first bit set or clear in a string", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 2, CommandInfo::Native,
                  &CommandsApi::BitPos),
    CommandHolder("BLPOP", "<key> [key ...] timeout",
                  "Remove and get the first element in a "
                  "list, or block until "
                  "one is available",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::BlPop),
    CommandHolder("BRPOP", "<key> [key ...] timeout",
                  "Remove and get the last element in a "
                  "list, or block until "
                  "one is available",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::BrPop),
    CommandHolder("BRPOPLPUSH", "source destination timeout",
                  "Pop a value from a list, push it to "
                  "another list and return "
                  "it; or block until "
                  "one is available",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::BrPopLpush),

    CommandHolder("CLIENT GETNAME", "-", "Get the current connection name",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ClientGetName),
    CommandHolder("CLIENT KILL",
                  "[ip:port] [ID client-id] [TYPE "
                  "normal|master|slave|pubsub] "
                  "[ADDR ip:port] [SKIPME yes/no]",
                  "Kill the connection of a client", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 0, 9, CommandInfo::Native,
                  &CommandsApi::ClientKill),
    CommandHolder("CLIENT LIST", "-", "Get the list of client connections",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ClientList),
    CommandHolder("CLIENT PAUSE", "<timeout>",
                  "Stop processing commands from clients "
                  "for some time",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::ClientPause),
    CommandHolder("CLIENT REPLY", "ON|OFF|SKIP",
                  "Instruct the server whether to reply to commands",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::ClientReply),
    CommandHolder("CLIENT SETNAME", "<connection-name>",
                  "Set the current connection name", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::ClientSetName),
#if defined(PRO_VERSION)
    CommandHolder("CLUSTER ADDSLOTS", "<slot> [slot ...]",
                  "Assign new hash slots to receiving node", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::ClusterAddSlots),
    CommandHolder("CLUSTER COUNT-FAILURE-REPORTS", "<node-id>",
                  "Return the number of failure reports "
                  "active for a given node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native,
                  &CommandsApi::ClusterCountFailureReports),
    CommandHolder("CLUSTER COUNTKEYSINSLOT", "<slot>",
                  "Return the number of local keys in the "
                  "specified hash slot",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::ClusterCountKeysSinSlot),
    CommandHolder("CLUSTER DELSLOTS", "<slot> [slot ...]",
                  "Set hash slots as unbound in receiving node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::ClusterDelSlots),
    CommandHolder("CLUSTER FAILOVER", "[FORCE|TAKEOVER]",
                  "Forces a slave to perform a manual "
                  "failover osyncf its master.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 1,
                  CommandInfo::Native, &CommandsApi::ClusterFailover),
    CommandHolder("CLUSTER FORGET", "<node-id>",
                  "Remove a node from the nodes table", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::ClusterForget),
    CommandHolder("CLUSTER GETKEYSINSLOT", "<slot> <count>",
                  "Return local key names in the specified hash slot",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::ClusterGetKeySinSlot),
    CommandHolder("CLUSTER INFO", "-",
                  "Provides info about Redis Cluster node state",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ClusterInfo),
    CommandHolder("CLUSTER KEYSLOT", "<key>",
                  "Returns the hash slot of the specified key", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::ClusterKeySlot),
    CommandHolder("CLUSTER MEET", "<ip> <port>",
                  "Force a node cluster to handshake with "
                  "another node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::ClusterMeet),
    CommandHolder("CLUSTER NODES", "-", "Get Cluster config for the node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ClusterNodes),
    CommandHolder("CLUSTER REPLICATE", "<node-id>",
                  "Reconfigure a node as a slave of the "
                  "specified master node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::ClusterReplicate),
    CommandHolder("CLUSTER RESET", "[HARD|SOFT]", "Reset a Redis Cluster node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 1,
                  CommandInfo::Native, &CommandsApi::ClusterReset),
    CommandHolder("CLUSTER SAVECONFIG", "-",
                  "Forces the node to save cluster state on disk",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ClusterSaveConfig),
    CommandHolder("CLUSTER SET-CONFIG-EPOCH", "<config-epoch>",
                  "Set the configuration epoch in a new node", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::ClusterSetConfigEpoch),
    CommandHolder("CLUSTER SETSLOT",
                  "<slot> IMPORTING|MIGRATING|STABLE|NODE [node-id]",
                  "Bind a hash slot to a specific node", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 2, CommandInfo::Native,
                  &CommandsApi::ClusterSetSlot),
    CommandHolder("CLUSTER SLAVES", "<node-id>",
                  "Licommon_execst slave nodes of the "
                  "specified master node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::ClusterSlaves),
    CommandHolder("CLUSTER SLOTS", "-",
                  "Get array of Cluster slot to node mappings", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 0, 0, CommandInfo::Native,
                  &CommandsApi::ClusterSlots),
#endif
    CommandHolder("COMMAND COUNT", "-", "Get total number of Redis commands",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::CommandCount),
    CommandHolder("COMMAND GETKEYS", "-",
                  "Extract keys given a full Redis command", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 0, 0, CommandInfo::Native,
                  &CommandsApi::CommandGetKeys),
    CommandHolder("COMMAND INFO", "command-name [command-name ...]",
                  "Get array of specific Redis command details",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::CommandInfo),
    CommandHolder("COMMAND", "-", "Get array of Redis command details",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Command),

    CommandHolder(DB_GET_CONFIG_COMMAND, "<parameter>",
                  "Get the value of a configuration parameter", UNDEFINED_SINCE,
                  DB_GET_CONFIG_COMMAND " databases", 1, 0, CommandInfo::Native,
                  &CommandsApi::ConfigGet),
    CommandHolder("CONFIG RESETSTAT", "-", "Reset the stats returned by INFO",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ConfigResetStat),
    CommandHolder("CONFIG REWRITE", "-",
                  "Rewrite the configuration file with the "
                  "in memory configuration",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ConfigRewrite),
    CommandHolder("CONFIG SET", "<parameter> <value>",
                  "Set a configuration parameter to the given value",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::ConfigSet),

    CommandHolder(DB_DBKCOUNT_COMMAND, "-",
                  "Return the number of keys in the "
                  "selected database",
                  UNDEFINED_SINCE, DB_DBKCOUNT_COMMAND, 0, 0,
                  CommandInfo::Native, &CommandsApi::DBkcount),
    CommandHolder("DBSIZE", "-",
                  "Return the number of keys in the "
                  "selected database",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::DbSize),

    CommandHolder("DEBUG OBJECT", "<key>",
                  "Get debugging information about a key", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::DebugObject),
    CommandHolder("DEBUG SEGFAULT", "-", "Make the server crash",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::DebugSegFault),

    CommandHolder("DECR", "<key>",
                  "Decrement the integer value of a key by one",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Decr),
    CommandHolder("DECRBY", "<key> <decrement>",
                  "Decrement the integer value of a key by "
                  "the given number",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::DecrBy),
    CommandHolder(DB_DELETE_KEY_COMMAND, "<key> [key ...]", "Delete a key",
                  UNDEFINED_SINCE, DB_DELETE_KEY_COMMAND " key", 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Delete),
    CommandHolder("DISCARD", "-", "Discard all commands issued after MULTI",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Discard),
    CommandHolder("DUMP", "<key>",
                  "Return a serialized version of the "
                  "value stored at the specified key.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Dump),
    CommandHolder("ECHO", "<message>", "Echo the given string", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::Echo),
    CommandHolder("EVAL", "script numkeys <key> [key ...] <arg> [arg ...]",
                  "Execute a Lua script server side", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Eval),
    CommandHolder("EVALSHA", "sha1 numkeys <key> [key ...] <arg> [arg ...]",
                  "Execute a Lua script server side", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::EvalSha),
    CommandHolder("EXEC", "-", "Execute all commands issued after MULTI",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Exec),
    CommandHolder("EXISTS", "key [key ...]", "Determine if a key exists",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Exists),
    CommandHolder(DB_SET_TTL_COMMAND, "<key> <seconds>",
                  "Set a key's time to live in seconds", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 0, CommandInfo::Native,
                  &CommandsApi::ExpireRedis),
    CommandHolder("EXPIREAT", "<key> <timestamp>",
                  "Set the expiration for a key as a UNIX timestamp",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::ExpireAt),
    CommandHolder("FLUSHALL", "-", "Remove all keys from all databases",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::FlushALL),
    CommandHolder(DB_FLUSHDB_COMMAND, "-",
                  "Remove all keys from the current database", UNDEFINED_SINCE,
                  DB_FLUSHDB_COMMAND, 0, 0, CommandInfo::Native,
                  &CommandsApi::FlushDB),
    CommandHolder("GEOADD",
                  "key longitude latitude member "
                  "[longitude latitude member ...",
                  "Add one or more geospatial items in the "
                  "geospatial index represented "
                  "using a sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 4, 0,
                  CommandInfo::Native, &CommandsApi::GeoAdd),
    CommandHolder("GEODIST", "key member1 member2 [unit]",
                  "Returns the distance between two "
                  "members of a geospatial index",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 1,
                  CommandInfo::Native, &CommandsApi::GeoDist),
    CommandHolder("GEOHASH", "key member [member ...]",
                  "Returns members of a geospatial index "
                  "as standard geohash strings",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::GeoHash),
    CommandHolder("GEOPOS", "key member [member ...]",
                  "Returns longitude and latitude of "
                  "members of a geospatial index",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::GeoPos),

    CommandHolder("GEORADIUS",
                  "key longitude latitude radius "
                  "m|km|ft|mi [WITHCOORD] "
                  "[WITHDIST] [WITHHASH] "
                  "[COUNT count] [ASC|DESC]",
                  "Query a sorted set representing a "
                  "geospatial index to fetch "
                  "members matching a "
                  "given maximum distance from a point",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 5, 6,
                  CommandInfo::Native, &CommandsApi::GeoRadius),
    CommandHolder("GEORADIUSBYMEMBER",
                  "key member radius m|km|ft|mi "
                  "[WITHCOORD] [WITHDIST] "
                  "[WITHHASH] [COUNT count] [ASC|DESC]",
                  "Query a sorted set representing a "
                  "geospatial index to fetch "
                  "members matching a given "
                  "maximum distance from a member",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 4, 6,
                  CommandInfo::Native, &CommandsApi::GeoRadiusByMember),
    CommandHolder(DB_GET_KEY_COMMAND, "<key>",
                  "Gecommon_exect the value of a key", UNDEFINED_SINCE,
                  DB_GET_KEY_COMMAND " key", 1, 0, CommandInfo::Native,
                  &CommandsApi::Get),
    CommandHolder("GETBIT", "<key> <offset>",
                  "Returns the bit value at offset in the "
                  "string value stored at key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::GetBit),
    CommandHolder("GETRANGE", "<key> <start> <end>",
                  "Get a substring of the string stored at a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::GetRange),
    CommandHolder("GETSET", "<key> <value>",
                  "Set the string value of a key and "
                  "return its old value",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::GetSet),
    CommandHolder("HDEL", "<key> <field> [field ...]",
                  "Delete one or more hash fields", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Hdel),
    CommandHolder("HEXISTS", "<key> <field>",
                  "Determine if a hash field exists", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 0, CommandInfo::Native,
                  &CommandsApi::Hexists),
    CommandHolder("HGET", "<key> <field>", "Get the value of a hash field",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Hget),
    CommandHolder("HGETALL", "<key>", "Get all the fields and values in a hash",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Hgetall),
    CommandHolder("HINCRBY", "<key> <field> <increment>",
                  "Increment the integer value of a hash "
                  "field by the given number",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::HincrByFloat),
    CommandHolder("HINCRBYFLOAT", "<key> <field> <increment>",
                  "Increment the float value of a hash "
                  "field by the given amount",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::HincrByFloat),
    CommandHolder("HKEYS", "<key>", "Get all the fields in a hash",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Hkeys),
    CommandHolder("HLEN", "<key>", "Get the number of fields in a hash",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Hlen),
    CommandHolder("HMGET", "<key> <field> [field ...]",
                  "Get the values of all the given hash fields",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Hmget),
    CommandHolder("HMSET", "<key> <field> <value> [field value ...]",
                  "Set multiple hash fields to multiple values",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Hmset,

                  {&TestArgsInRange, &TestArgsModule2Equal1}),
    CommandHolder("HSCAN", "<key> <cursor> [MATCH pattern] [COUNT count]",
                  "Incrementally iterate hash fields and associated "
                  "values",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 4,
                  CommandInfo::Native, &CommandsApi::Hscan),
    CommandHolder("HSET", "<key> <field> <value>",
                  "Set the string value of a hash field", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 3, 0, CommandInfo::Native,
                  &CommandsApi::Hset),
    CommandHolder("HSETNX", "<key> <field> <value>",
                  "Set the value of a hash field, only if "
                  "the field does not exist",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::HsetNX),
    CommandHolder("HSTRLEN", "<key> <field>",
                  "Get the length of the value of a hash field",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Hstrlen),
    CommandHolder("HVALS", "<key>", "Get all the values in a hash",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Hvals),
    CommandHolder("INCR", "<key>",
                  "Increment the integer value of a key by one",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Incr),
    CommandHolder("INCRBY", "<key> <increment>",
                  "Increment the integer value of a key by "
                  "the given amount",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::IncrBy),
    CommandHolder("INCRBYFLOAT", "<key> <increment>",
                  "Increment the float value of a key by "
                  "the given amount",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::IncrByFloat),
    CommandHolder(DB_KEYS_COMMAND, "<pattern>",
                  "Find all keys matching the given pattern", UNDEFINED_SINCE,
                  DB_KEYS_COMMAND " a z 10", 1, 0, CommandInfo::Native,
                  &CommandsApi::RKeys),
    CommandHolder("LASTSAVE", "-",
                  "Get the UNIX time stamp of the last "
                  "successful save to disk",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::LastSave),
    CommandHolder("LINDEX", "<key> <index>",
                  "Get an element from a list by its index", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 0, CommandInfo::Native,
                  &CommandsApi::Lindex),
    CommandHolder("LINSERT", "<key> <BEFORE|AFTER> <pivot value>",
                  "Insert an element before or after "
                  "another element in a list",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::Linsert),
    CommandHolder("LLEN", "<key>", "Get the length of a list", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::Llen),
    CommandHolder("LPOP", "<key>", "Remove and get the first element in a list",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Lpop),
    CommandHolder("LPUSH", "<key> <value> [value ...]",
                  "Prepend one or multiple values to a list", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Lpush),
    CommandHolder("LPUSHX", "<key> <value>",
                  "Prepend a value to a list, only if the "
                  "list exists",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::LpushX),
    CommandHolder("LRANGE", "<key> <start> <stop>",
                  "Get a range of elements from a list", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 3, 0, CommandInfo::Native,
                  &CommandsApi::Lrange),
    CommandHolder("LREM", "<key> <count> <value>",
                  "Remove elements from a list", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 3, 0, CommandInfo::Native,
                  &CommandsApi::Lrem),
    CommandHolder("LSET", "<key> <index> <value>",
                  "Set the value of an element in a list "
                  "by its index",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::Lset),
    CommandHolder("LTRIM", "<key> <start> <stop>",
                  "Trim a list to the specified range", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 3, 0, CommandInfo::Native,
                  &CommandsApi::Ltrim),
    CommandHolder("MGET", "<key> [key ...]",
                  "Get the values of all the given keys", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Mget),
    CommandHolder("MIGRATE",
                  "<host> <port> <key> <destination-db> <timeout> [COPY] "
                  "[REPLACE] [KEYS key]",
                  "Atomically transfer a key from a Redis instance "
                  "to another one.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 5, 2,
                  CommandInfo::Native, &CommandsApi::Migrate),
    CommandHolder("MONITOR", "-",
                  "Listen for all requests received by the "
                  "server in real time",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Monitor),
    CommandHolder("MOVE", "<key> <db>", "Move a key to another database",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Move),
    CommandHolder("MSET", "<key> <value> [key value ...]",
                  "Set multiple keys to multiple values", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Mset),
    CommandHolder("MSETNX", "<key> <value> [key value ...]",
                  "Set multiple keys to multiple values, "
                  "only if none of the keys exist",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::MsetNX),
    CommandHolder("MULTI", "-", "Mark the start of a transaction block",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Multi),
    CommandHolder("OBJECT", "<subcommand> [arguments [arguments ...]]",
                  "Inspect the internals of Redis objects", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Object),
    CommandHolder("PERSIST", "<key>", "Remove the expiration from a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Persist),
    CommandHolder(REDIS_CHANGE_PTTL_COMMAND, "<key> <milliseconds>",
                  "Set a key's time to live in milliseconds", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 0, CommandInfo::Native,
                  &CommandsApi::Pexpire),
    CommandHolder("PEXPIREAT", "<key> <milliseconds-timestamp>",
                  "Set the expiration for a key as a UNIX "
                  "timestamp specified "
                  "in milliseconds",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::PexpireAt),
    CommandHolder("PFADD", "<key> <element> [element ...]",
                  "Adds the specified elements to the "
                  "specified HyperLogLog.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Pfadd),
    CommandHolder("PFCOUNT", "<key> [key ...]",
                  "Return the approximated cardinality of "
                  "the set(s) observed "
                  "by the HyperLogLog at key(s).",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Pfcount),
    CommandHolder("PFMERGE", "<destkey> <sourcekey> [sourcekey ...]",
                  "Merge N different HyperLogLogs into a single one.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Pfmerge),
    CommandHolder("PING", "[message]", "Ping the server", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 0, 1, CommandInfo::Native,
                  &CommandsApi::Ping),
    CommandHolder("PSETEX", "<key> <milliseconds> <value>",
                  "Set the value and expiration in "
                  "milliseconds of a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::PsetEx),
    CommandHolder("PSUBSCRIBE", "<pattern> [pattern ...]",
                  "Listen for messages published to "
                  "channels matching the given patterns",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Subscribe),
    CommandHolder(REDIS_GET_PTTL_COMMAND, "<key>",
                  "Get the time to live for a key in milliseconds",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Pttl),
    CommandHolder(DB_PUBLISH_COMMAND, "<channel> <message>",
                  "Post a message to a channel", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 0, CommandInfo::Native,
                  &CommandsApi::Publish),
    CommandHolder("PUBSUB", "<subcommand> [argument [argument ...]]",
                  "Inspect the state of the Pub/Sub subsystem", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::PubSub),
    CommandHolder("PUNSUBSCRIBE", "[pattern [pattern ...]]",
                  "Stop listening for messages posted to "
                  "channels matching the "
                  "given patterns",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::PunSubscribe),
    CommandHolder(DB_QUIT_COMMAND, "-", "Close the connection", UNDEFINED_SINCE,
                  DB_QUIT_COMMAND, 0, 0, CommandInfo::Native,
                  &CommandsApi::Quit),
    CommandHolder("RANDOMKEY", "-", "Return a random key from the keyspace",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::RandomKey),
    CommandHolder("READONLY", "-",
                  "Enables read queries for a connection "
                  "to a cluster slave node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ReadOnly),
    CommandHolder("READWRITE", "-",
                  "Disables read queries for a connection "
                  "to a cluster slave node",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ReadWrite),
    CommandHolder(DB_RENAME_KEY_COMMAND, "<key> <newkey>", "Rename a key",
                  UNDEFINED_SINCE, DB_RENAME_KEY_COMMAND " old_name new_name",
                  2, 0, CommandInfo::Native, &CommandsApi::Rename),
    CommandHolder("RENAMENX", "<key> <newkey>",
                  "Rename a key, only if the new key does not exist",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::RenameNx),
    CommandHolder("RESTORE", "<key> <ttl> <serialized-value> [REPLACE]",
                  "Create a key using the provided serialized value, "
                  "previously obtained using DUMP.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 1,
                  CommandInfo::Native, &CommandsApi::Restore),
    CommandHolder("ROLE", "-",
                  "Return the role of the instance in the "
                  "context of replication",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Role),
    CommandHolder("RPOP", "<key>", "Remove and get the last element in a list",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Rpop),
    CommandHolder("RPOPLPUSH", "<source> <destination>",
                  "Remove the last element in a list, prepend it to another "
                  "list and return it",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::RpopLpush),
    CommandHolder("RPUSH", "<key> <value> [value ...]",
                  "Append one or multiple values to a list", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Rpush),
    CommandHolder("RPUSHX", "<key> <value>",
                  "Append a value to a list, only if the list exists",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::RpushX),
    CommandHolder("SADD", "<key> <member> [member ...]",
                  "Add one or more members to a set", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Sadd),
    CommandHolder("SAVE", "-", "Synchronously save the dataset to disk",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Save),
    CommandHolder(DB_SCAN_COMMAND, "<cursor> [MATCH pattern] [COUNT count]",
                  "Incrementally iterate the keys space", UNDEFINED_SINCE,
                  DB_SCAN_COMMAND " 0 MATCH * COUNT 10", 1, 4,
                  CommandInfo::Native, &CommandsApi::Scan),
    CommandHolder(DB_JSONDUMP_COMMAND,
                  "<cursor> PATH <absolute_path> [MATCH pattern] [COUNT count]",
                  "Dump DB into json file by path.", UNDEFINED_SINCE,
                  DB_JSONDUMP_COMMAND " 0 PATH ~/dump.json MATCH * COUNT 10", 3,
                  4, CommandInfo::Native, &CommandsApi::JsonDump),
    CommandHolder("SCARD", "<key>", "Get the number of members in a set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Scard),

    CommandHolder("SCRIPT DEBUG", "<YES|SYNC|NO>",
                  "Set the debug mode for executed scripts.", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 0, CommandInfo::Native,
                  &CommandsApi::ScriptDebug),
    CommandHolder("SCRIPT EXISTS", "script [script ...]",
                  "Check existence of scripts in the script cache.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::ScriptExists),
    CommandHolder("SCRIPT FLUSH", "-",
                  "Remove all the scripts from the script cache.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ScriptFlush),
    CommandHolder("SCRIPT KILL", "-", "Kill the script currently in execution.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::ScriptKill),
    CommandHolder("SCRIPT LOAD", "<script>",
                  "Load the specified Lua script into the "
                  "script cache.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::ScriptLoad),

    CommandHolder("SDIFF", "<key> [key ...]", "Subtract multiple sets",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Sdiff),
    CommandHolder("SDIFFSTORE", "<destination> <key> [key ...]",
                  "Subtract multiple sets and store the "
                  "resulting set in a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::SdiffStore),
    CommandHolder(DB_SELECTDB_COMMAND, "<index>",
                  "Change the selected database for the "
                  "current connection",
                  UNDEFINED_SINCE, DB_SELECTDB_COMMAND " 0", 1, 0,
                  CommandInfo::Native, &CommandsApi::Select),
    CommandHolder(DB_SET_KEY_COMMAND,
                  "<key> <value> [EX seconds] [PX milliseconds] [NX|XX]",
                  "Set the string value of a key", UNDEFINED_SINCE,
                  DB_SET_KEY_COMMAND " key value", 2, 5, CommandInfo::Native,
                  &CommandsApi::Set),
    CommandHolder("SETBIT", "<key> <offset> <value>",
                  "Sets or clears the bit at offset in the "
                  "string value stored at key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::SetBit),
    CommandHolder("SETEX", "<key> <seconds> <value>",
                  "Set the value and expiration of a key", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 3, 0, CommandInfo::Native,
                  &CommandsApi::SetEx),
    CommandHolder("SETNX", "<key> <value>",
                  "Set the value of a key, only if the key "
                  "does not exist",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::SetNX),
    CommandHolder("SETRANGE", "<key> <offset> <value>",
                  "Overwrite part of a string at key "
                  "starting at the specified offset",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::SetRange),
    CommandHolder("SHUTDOWN", "[NOSAVE|SAVE]",
                  "Synchronously save the dataset to disk "
                  "and then shut down the server",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 1,
                  CommandInfo::Native, &CommandsApi::Shutdown),
    CommandHolder("SINTER", "<key> [key ...]", "Intersect multiple sets",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Sinter),
    CommandHolder("SINTERSTORE", "<destination> <key> [key ...]",
                  "Intersect multiple sets and store the "
                  "resulting set in a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::SinterStore),
    CommandHolder("SISMEMBER", "<key> <member>",
                  "Determine if a given value is a member of a set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::SisMember),
    CommandHolder("SLAVEOF", "<host> <port>",
                  "Make the server a slave of another "
                  "instance, or promote it as master",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::SlaveOf),
    CommandHolder("SLOWLOG", "<subcommand> [argument]",
                  "Manages the Redis slow queries log", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 1, 1, CommandInfo::Native,
                  &CommandsApi::SlowLog),
    CommandHolder("SMEMBERS", "<key>", "Get all the members in a set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Smembers),
    CommandHolder("SMOVE", "<source> <destination> <member>",
                  "Move a member from one set to another", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 3, 0, CommandInfo::Native,
                  &CommandsApi::Smove),
    CommandHolder("SORT",
                  "<key> [BY pattern] [LIMIT offset count] [GET "
                  "pattern [GET "
                  "pattern ...]] "
                  "[ASC|DESC] [ALPHA] [STORE destination]",
                  "Sort the elements in a list, set or sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Sort),
    CommandHolder("SPOP", "<key> [count]",
                  "Remove and return one or multiple random members from a set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 1,
                  CommandInfo::Native, &CommandsApi::Spop),
    CommandHolder("SRANDMEMBER", "<key> [count]",
                  "Get one or multiple random members from a set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 1,
                  CommandInfo::Native, &CommandsApi::SRandMember),
    CommandHolder("SREM", "<key> <member> [member ...]",
                  "Remove one or more members from a set", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, INFINITE_COMMAND_ARGS,
                  CommandInfo::Native, &CommandsApi::Srem),
    CommandHolder("SSCAN", "<key> <cursor> [MATCH pattern] [COUNT count]",
                  "Incrementally iterate Set elements", UNDEFINED_SINCE,
                  UNDEFINED_EXAMPLE_STR, 2, 4, CommandInfo::Native,
                  &CommandsApi::Sscan),
    CommandHolder("STRLEN", "<key>",
                  "Get the length of the value stored in a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::StrLen),
    CommandHolder(DB_SUBSCRIBE_COMMAND, "<channel> [channel ...]",
                  "Listen for messages published to the "
                  "given channels",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Subscribe),
    CommandHolder("SUNION", "<key> [key ...]", "Add multiple sets",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Sunion),
    CommandHolder("SUNIONSTORE", "<destination> <key> [key ...]",
                  "Add multiple sets and store the "
                  "resulting set in a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::SunionStore),
    CommandHolder("SYNC", "-", "Internal command used for replication",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Sync),
    CommandHolder("PSYNC", "-", "Internal command used for replication",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Sync),
    CommandHolder("TIME", "-", "Return the current server time",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Time),
    CommandHolder(DB_GET_TTL_COMMAND, "<key>", "Get the time to live for a key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::GetTTL),
    CommandHolder("TYPE", "<key>", "Determine the type stored at key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Type),
    CommandHolder("UNSUBSCRIBE", "[channel [channel ...]]",
                  "Stop listening for messages posted to "
                  "the given channels",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Unsubscribe),
    CommandHolder("UNWATCH", "-", "Forget about all watched keys",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::Unwatch),
    CommandHolder("WAIT", "<numslaves> <timeout>",
                  "Wait for the synchronous replication of "
                  "all the write "
                  "commands sent in the "
                  "context of the current connection",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Wait),
    CommandHolder("WATCH", "key [key ...]",
                  "Watch the given keys to determine "
                  "execution of the MULTI/EXEC block",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Watch),
    CommandHolder("ZADD",
                  "<key> [NX|XX] [CH] [INCR] <score> "
                  "<member> [score member ...]",
                  "Add one or more members to a sorted "
                  "set, or update its score if it "
                  "already exists",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Zadd),
    CommandHolder("ZCARD", "<key>", "Get the number of members in a sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::Zcard),
    CommandHolder("ZCOUNT", "<key> <min> <max>",
                  "Count the members in a sorted set with "
                  "scores within the given values",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::Zcount),
    CommandHolder("ZINCRBY", "<key> <increment> <member>",
                  "Increment the score of a member in a sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::ZincrBy),
    CommandHolder("ZINTERSTORE",
                  "<destination> <numkeys> <key> [key ...] "
                  "[WEIGHTS weight] "
                  "[AGGREGATE SUM|MIN|MAX]",
                  "Intersect multiple sorted sets and "
                  "store the resulting "
                  "sorted set in a new key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::ZincrStore),
    CommandHolder("ZLEXCOUNT", "<key> <min> <max>",
                  "Count the number of members in a sorted "
                  "set between a given "
                  "lexicographical range",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::ZlexCount),
    CommandHolder("ZRANGE", "<key> <start> <stop> [WITHSCORES]",
                  "Return a range of members in a sorted "
                  "set, by index",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 1,
                  CommandInfo::Native, &CommandsApi::Zrange),
    CommandHolder("ZRANGEBYLEX", "<key> <min> <max> [LIMIT offset count]",
                  "Return a range of members in a sorted "
                  "set, by lexicographical range",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 3,
                  CommandInfo::Native, &CommandsApi::ZrangeByLex),
    CommandHolder("ZRANGEBYSCORE",
                  "<key> <min> <max> [WITHSCORES] [LIMIT "
                  "offset count]",
                  "Return a range of members in a sorted "
                  "set, by score",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 4,
                  CommandInfo::Native, &CommandsApi::ZrangeByScore),
    CommandHolder("ZRANK", "<key> <member>",
                  "Determine the index of a member in a sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Zrank),
    CommandHolder("ZREM", "<key> <member> [member ...]",
                  "Remove one or more members from a sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::Zrem),
    CommandHolder("ZREMRANGEBYLEX", "<key> <min> <max>",
                  "Remove all members in a sorted set "
                  "between the given "
                  "lexicographical range",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::ZremRangeByLex),
    CommandHolder("ZREMRANGEBYRANK", "<key> <start> <stop>",
                  "Remove all members in a sorted set "
                  "within the given indexes",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::ZremRangeByRank),
    CommandHolder("ZREMRANGEBYSCORE", "<key> <min> <max>",
                  "Remove all members in a sorted set "
                  "within the given scores",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::ZremRangeByScore),
    CommandHolder("ZREVRANGE", "<key> <start> <stop> [WITHSCORES]",
                  "Return a range of members in a sorted "
                  "set, by index, with "
                  "scores ordered from high to low",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 1,
                  CommandInfo::Native, &CommandsApi::ZrevRange),
    CommandHolder("ZREVRANGEBYLEX", "<key> <max> <min> [LIMIT offset count]",
                  "Return a range of members in a sorted set, by "
                  "lexicographical range, ordered "
                  "from higher to lower strings.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 3,
                  CommandInfo::Native, &CommandsApi::ZrevRangeByLex),
    CommandHolder("ZREVRANGEBYSCORE",
                  "<key> <max> <min> [WITHSCORES] [LIMIT "
                  "offset count]",
                  "Return a range of members in a sorted "
                  "set, by score, with "
                  "scores ordered from high to low",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 4,
                  CommandInfo::Native, &CommandsApi::ZrevRangeByScore),
    CommandHolder("ZREVRANK", "<key> <member>",
                  "Determine the index of a member in a "
                  "sorted set, with "
                  "scores ordered from high to low",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::ZrevRank),
    CommandHolder("ZSCAN", "<key> <cursor> [MATCH pattern] [COUNT count]",
                  "Incrementally iterate sorted sets elements and "
                  "associated scores",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 4,
                  CommandInfo::Native, &CommandsApi::Zscan),
    CommandHolder("ZSCORE", "<key> <member>",
                  "Get the score associated with the given "
                  "member in a sorted set",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 2, 0,
                  CommandInfo::Native, &CommandsApi::Zscore),
    CommandHolder("ZUNIONSTORE",
                  "<destination> <numkeys> <key> [key ...] "
                  "[WEIGHTS weight] "
                  "[AGGREGATE SUM|MIN|MAX]",
                  "Add multiple sorted sets and store the "
                  "resulting sorted set "
                  "in a new key",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3,
                  INFINITE_COMMAND_ARGS, CommandInfo::Native,
                  &CommandsApi::ZunionStore),
#if defined(PRO_VERSION)
    CommandHolder("SENTINEL MASTERS", "-",
                  "Show a list of monitored masters and their state.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::SentinelMasters),
    CommandHolder("SENTINEL MASTER", "<master name>",
                  "Show the state and info of the specified master.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelMaster),
    CommandHolder("SENTINEL SLAVES", "<master name>",
                  "Show a list of slaves for this master, "
                  "and their state.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelSlaves),
    CommandHolder("SENTINEL SENTINELS", "<master name>",
                  "Show a list of sentinel instances for "
                  "this master, and their state.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelSentinels),
    CommandHolder("SENTINEL GET-MASTER-ADDR-BY-NAME", "<master name>",
                  "Return the ip and port number of the "
                  "master with that name.\n"
                  "If a failover is in progress or "
                  "terminated successfully for this "
                  "master "
                  "it returns the address and port of the "
                  "promoted slave.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native,
                  &CommandsApi::SentinelGetMasterAddrByName),
    CommandHolder("SENTINEL RESET", "<pattern>",
                  "This command will reset all the masters with "
                  "matching name.\n"
                  "The pattern argument is a glob-style pattern.\n"
                  "The reset process clears any previous state in a "
                  "master (including a "
                  "failover in "
                  "progress), "
                  "and removes every slave and sentinel already "
                  "discovered and "
                  "associated with the master.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelReset),
    CommandHolder("SENTINEL FAILOVER", "<master name>",
                  "Force a failover as if the master was not "
                  "reachable, "
                  "and without asking for agreement to other "
                  "GetSentinels "
                  "(however a new version of the configuration will "
                  "be "
                  "published so that the other "
                  "GetSentinels will update their configurations).",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelFailover),
    CommandHolder("SENTINEL CKQUORUM", "<master name>",
                  "Check if the current Sentinel "
                  "configuration is able to "
                  "reach the quorum needed "
                  "to failover a master, "
                  "and the majority needed to authorize "
                  "the failover.\n"
                  "This command should be used in "
                  "monitoring systems to check "
                  "if a Sentinel "
                  "deployment is ok.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelCkquorum),
    CommandHolder("SENTINEL FLUSHCONFIG", "-",
                  "Force Sentinel to rewrite its "
                  "configuration on disk, including the "
                  "current Sentinel "
                  "state.\n"
                  "Normally Sentinel rewrites the "
                  "configuration every time something "
                  "changes in its state "
                  "(in the context of the subset of the "
                  "state which is persisted on disk "
                  "across restart).\n"
                  "However sometimes it is possible that "
                  "the configuration file is lost "
                  "because of operation "
                  "errors, "
                  "disk failures, package upgrade scripts "
                  "or configuration managers.\n"
                  "In those cases a way to to force "
                  "Sentinel to rewrite the "
                  "configuration file is handy.\n"
                  "This command works even if the previous "
                  "configuration file is "
                  "completely missing.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 0, 0,
                  CommandInfo::Native, &CommandsApi::SentinelFlushConfig),
    CommandHolder("SENTINEL MONITOR", "<name> <ip> <port> <quorum>",
                  "This command tells the Sentinel to "
                  "start monitoring a new master with "
                  "the "
                  "specified name, ip, port, and quorum.\n"
                  "It is identical to the sentinel monitor "
                  "configuration directive in "
                  "sentinel.conf configuration file, "
                  "with the difference that you can't use "
                  "an hostname in as ip, but you "
                  "need to "
                  "provide an IPv4 or IPv6 address.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 4, 0,
                  CommandInfo::Native, &CommandsApi::SentinelMonitor),
    CommandHolder("SENTINEL REMOVE", "<name>",
                  "Used in order to remove the specified "
                  "master: the master will no "
                  "longer be monitored, "
                  "and will totally be removed from the "
                  "internal state of the Sentinel, "
                  "so it will no longer listed by SENTINEL "
                  "masters and so forth.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 1, 0,
                  CommandInfo::Native, &CommandsApi::SentinelRemove),
    CommandHolder("SENTINEL SET", "<name> <option> <value>",
                  "The SET command is very similar to the "
                  "CONFIG SET command of Redis, "
                  "and is used in order to change "
                  "configuration parameters of a specific "
                  "master.\n"
                  "Multiple option / value pairs can be "
                  "specified (or none at all).\n"
                  "All the configuration parameters that "
                  "can be configured via "
                  "sentinel.conf are also configurable "
                  "using the SET command.",
                  UNDEFINED_SINCE, UNDEFINED_EXAMPLE_STR, 3, 0,
                  CommandInfo::Native, &CommandsApi::SentinelSet)
#endif
};

} // namespace
} // namespace pika
template <> const char *ConnectionTraits<PIKA>::GetBasedOn() {
  return "hiredis";
}

template <> const char *ConnectionTraits<PIKA>::GetVersionApi() {
  return redis_compatible::GetHiredisVersion();
}

template <>
const ConstantCommandsArray &ConnectionCommandsTraits<PIKA>::GetCommands() {
  return pika::g_commands;
}
namespace internal {
template <>
common::Error
ConnectionAllocatorTraits<pika::NativeConnection, pika::RConfig>::Connect(
    const pika::RConfig &config, pika::NativeConnection **hout) {
  pika::NativeConnection *context = nullptr;
  common::Error err = pika::CreateConnection(config, &context);
  if (err) {
    return err;
  }

  *hout = context;
  // redisEnableKeepAlive(context);
  return common::Error();
}

template <>
common::Error
ConnectionAllocatorTraits<pika::NativeConnection, pika::RConfig>::Disconnect(
    pika::NativeConnection **handle) {
  pika::NativeConnection *lhandle = *handle;
  if (lhandle) {
    redisFree(lhandle);
  }
  lhandle = nullptr;
  return common::Error();
}

template <>
bool ConnectionAllocatorTraits<pika::NativeConnection, pika::RConfig>::
    IsConnected(pika::NativeConnection *handle) {
  if (!handle) {
    return false;
  }

  return true;
}

} // namespace internal

namespace pika {

common::Error CreateConnection(const RConfig &config,
                               NativeConnection **context) {
  return redis_compatible::CreateConnection(config, config.ssh_info, context);
}

common::Error TestConnection(const RConfig &config) {
  return redis_compatible::TestConnection(config, config.ssh_info);
}

#if defined(PRO_VERSION)
common::Error
DiscoveryClusterConnection(const RConfig &config,
                           std::vector<ServerDiscoveryClusterInfoSPtr> *infos) {
  return redis_compatible::DiscoveryClusterConnection(config, config.ssh_info,
                                                      infos);
}

common::Error DiscoverySentinelConnection(
    const RConfig &config,
    std::vector<ServerDiscoverySentinelInfoSPtr> *infos) {
  return redis_compatible::DiscoverySentinelConnection(config, config.ssh_info,
                                                       infos);
}
#endif

DBConnection::DBConnection(CDBConnectionClient *client) : base_class(client) {}

common::Error DBConnection::DBkcountImpl(size_t *size) {
  redisReply *reply = reinterpret_cast<redisReply *>(
      redisCommand(base_class::connection_.handle_, DBSIZE));

  if (!reply || reply->type != REDIS_REPLY_STRING) {
    return common::make_error("Couldn't determine " DB_DBKCOUNT_COMMAND "!");
  }

  ServerInfo::KeySpace ks(reply->str);
  /* Grab the number of keys and free our reply */
  *size = ks.kv_ + ks.hash_ + ks.list_ + ks.set_ + ks.zset_;
  freeReplyObject(reply);
  return common::Error();
}

} // namespace pika
} // namespace core
} // namespace fastonosql
