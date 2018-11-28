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

#include "core/db/pika/internal/commands_api.h"

#include <string>
#include <vector>

#include <common/string_util.h>

#include <fastonosql/core/db/pika/db_connection.h>

#include <fastonosql/core/value.h>

namespace fastonosql {
namespace core {
namespace pika {

common::Error CommandsApi::Append(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("APPEND")}, argv), out);
}

common::Error CommandsApi::BgRewriteAof(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BGREWRITEAOF")}, argv), out);
}

common::Error CommandsApi::BgSave(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BGSAVE")}, argv), out);
}

common::Error CommandsApi::BitCount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BITCOUNT")}, argv), out);
}

common::Error CommandsApi::BitField(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BITFIELD")}, argv), out);
}

common::Error CommandsApi::BitOp(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BITOP")}, argv), out);
}

common::Error CommandsApi::BitPos(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BITPOS")}, argv), out);
}

common::Error CommandsApi::BlPop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BLPOP")}, argv), out);
}

common::Error CommandsApi::BrPop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BRPOP")}, argv), out);
}

common::Error CommandsApi::BrPopLpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BRPOPLPUSH")}, argv), out);
}

common::Error CommandsApi::ClientGetName(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("GETNAME")}, argv),
                         out);
}

common::Error CommandsApi::ClientKill(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("KILL")}, argv),
                         out);
}

common::Error CommandsApi::ClientList(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("LIST")}, argv),
                         out);
}

common::Error CommandsApi::ClientPause(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("PAUSE")}, argv),
                         out);
}

common::Error CommandsApi::ClientReply(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("REPLY")}, argv),
                         out);
}

common::Error CommandsApi::ClusterAddSlots(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("ADDSLOTS")}, argv),
                         out);
}

common::Error CommandsApi::ClusterCountFailureReports(internal::CommandHandler* handler,
                                                      commands_args_t argv,
                                                      FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("COUNT-FAILURE-REPORTS")}, argv), out);
}

common::Error CommandsApi::ClusterCountKeysSinSlot(internal::CommandHandler* handler,
                                                   commands_args_t argv,
                                                   FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("CLIENT"), GEN_CMD_STRING("COUNTKEYSINSLOT")}, argv), out);
}

common::Error CommandsApi::ClusterDelSlots(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("DELSLOTS")}, argv),
                         out);
}

common::Error CommandsApi::ClusterFailover(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("FAILOVER")}, argv),
                         out);
}

common::Error CommandsApi::ClusterForget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("FORGET")}, argv),
                         out);
}

common::Error CommandsApi::ClusterGetKeySinSlot(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("GETKEYSINSLOT")}, argv), out);
}

common::Error CommandsApi::ClusterInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("INFO")}, argv),
                         out);
}

common::Error CommandsApi::ClusterKeySlot(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("KEYSLOT")}, argv),
                         out);
}

common::Error CommandsApi::ClusterMeet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("MEET")}, argv),
                         out);
}

common::Error CommandsApi::ClusterNodes(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("NODES")}, argv),
                         out);
}

common::Error CommandsApi::ClusterReplicate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("REPLICATE")}, argv), out);
}

common::Error CommandsApi::ClusterReset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("RESET")}, argv),
                         out);
}

common::Error CommandsApi::ClusterSaveConfig(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("SAVECONFIG")}, argv), out);
}

common::Error CommandsApi::ClusterSetConfigEpoch(internal::CommandHandler* handler,
                                                 commands_args_t argv,
                                                 FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("SET-CONFIG-EPOCH")}, argv), out);
}

common::Error CommandsApi::ClusterSetSlot(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("SETSLOT")}, argv),
                         out);
}

common::Error CommandsApi::ClusterSlaves(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("SLAVES")}, argv),
                         out);
}

common::Error CommandsApi::ClusterSlots(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CLUSTER"), GEN_CMD_STRING("SLOTS")}, argv),
                         out);
}

common::Error CommandsApi::CommandCount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("COMMAND"), GEN_CMD_STRING("COUNT")}, argv),
                         out);
}

common::Error CommandsApi::CommandGetKeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("COMMAND"), GEN_CMD_STRING("GETKEYS")}, argv),
                         out);
}

common::Error CommandsApi::CommandInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("COMMAND"), GEN_CMD_STRING("INFO")}, argv),
                         out);
}

common::Error CommandsApi::Command(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("COMMAND")}, argv), out);
}

common::Error CommandsApi::ConfigGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  if (common::EqualsASCII(argv[0], GEN_CMD_STRING("databases"), false)) {
    db_names_t dbs;
    DBConnection* red = static_cast<DBConnection*>(handler);
    common::Error err = red->ConfigGetDatabases(&dbs);
    if (err) {
      return err;
    }

    common::ArrayValue* arr = new common::ArrayValue;
    arr->AppendStrings(dbs);
    FastoObject* child = new FastoObject(out, arr, red->GetDelimiter());
    out->AddChildren(child);
    return common::Error();
  }

  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CONFIG"), GEN_CMD_STRING("GET")}, argv), out);
}

common::Error CommandsApi::ConfigResetStat(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CONFIG"), GEN_CMD_STRING("RESETSTAT")}, argv),
                         out);
}

common::Error CommandsApi::ConfigRewrite(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CONFIG"), GEN_CMD_STRING("REWRITE")}, argv),
                         out);
}

common::Error CommandsApi::ConfigSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("CONFIG"), GEN_CMD_STRING("SET")}, argv), out);
}

common::Error CommandsApi::DebugObject(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("DEBUG"), GEN_CMD_STRING("OBJECT")}, argv),
                         out);
}

common::Error CommandsApi::DebugSegFault(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("DEBUG"), GEN_CMD_STRING("SEGFAULT")}, argv),
                         out);
}

common::Error CommandsApi::Discard(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);

  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("DISCARD")}, argv), out);
}

common::Error CommandsApi::Dump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("DUMP")}, argv), out);
}

common::Error CommandsApi::Echo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ECHO")}, argv), out);
}

common::Error CommandsApi::Eval(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("EVAL")}, argv), out);
}

common::Error CommandsApi::EvalSha(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("EVALSHA")}, argv), out);
}

common::Error CommandsApi::Exec(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("EXEC")}, argv), out);
}

common::Error CommandsApi::Exists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("EXISTS")}, argv), out);
}

common::Error CommandsApi::ExpireAt(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("EXPIREAT")}, argv), out);
}

common::Error CommandsApi::FlushALL(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("FLUSHALL")}, argv), out);
}

common::Error CommandsApi::GeoAdd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEOADD")}, argv), out);
}

common::Error CommandsApi::GeoDist(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEODIST")}, argv), out);
}

common::Error CommandsApi::GeoHash(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEOHASH")}, argv), out);
}

common::Error CommandsApi::GeoPos(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEOPOS")}, argv), out);
}

common::Error CommandsApi::GeoRadius(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEORADIUS")}, argv), out);
}

common::Error CommandsApi::GeoRadiusByMember(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEORADIUSBYMEMBER")}, argv), out);
}

common::Error CommandsApi::GetBit(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GETBIT")}, argv), out);
}

common::Error CommandsApi::GetRange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GETRANGE")}, argv), out);
}

common::Error CommandsApi::GetSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GETSET")}, argv), out);
}

common::Error CommandsApi::Hdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HDEL")}, argv), out);
}

common::Error CommandsApi::Hexists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HEXISTS")}, argv), out);
}

common::Error CommandsApi::Hget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HGET")}, argv), out);
}

common::Error CommandsApi::HincrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HINCRBY")}, argv), out);
}

common::Error CommandsApi::HincrByFloat(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HINCRBYFLOAT")}, argv), out);
}

common::Error CommandsApi::Hkeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HKEYS")}, argv), out);
}

common::Error CommandsApi::Hlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HLEN")}, argv), out);
}

common::Error CommandsApi::Hmget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HMGET")}, argv), out);
}

common::Error CommandsApi::Hscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HSCAN")}, argv), out);
}

common::Error CommandsApi::Hset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HSET")}, argv), out);
}

common::Error CommandsApi::HsetNX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HSETNX")}, argv), out);
}

common::Error CommandsApi::Hstrlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HSTRLEN")}, argv), out);
}

common::Error CommandsApi::Hvals(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HVALS")}, argv), out);
}

common::Error CommandsApi::RKeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(DB_KEYS_COMMAND)}, argv), out);
}

common::Error CommandsApi::LastSave(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LASTSAVE")}, argv), out);
}

common::Error CommandsApi::Lindex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LINDEX")}, argv), out);
}

common::Error CommandsApi::Linsert(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LINSERT")}, argv), out);
}

common::Error CommandsApi::Llen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LLEN")}, argv), out);
}

common::Error CommandsApi::Lpop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LPOP")}, argv), out);
}

common::Error CommandsApi::LpushX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LPUSHX")}, argv), out);
}

common::Error CommandsApi::Lrem(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LREM")}, argv), out);
}

common::Error CommandsApi::Lset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LSET")}, argv), out);
}

common::Error CommandsApi::Ltrim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LTRIM")}, argv), out);
}

common::Error CommandsApi::Mget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  std::vector<NKey> keys;
  for (size_t i = 0; i < argv.size(); ++i) {
    const nkey_t key_str(argv[i]);
    const NKey key(key_str);

    keys.push_back(key);
  }

  std::vector<NDbKValue> dbv;
  common::Error err = red->Mget(keys, &dbv);
  if (err) {
    return err;
  }

  common::ArrayValue* arr = common::Value::CreateArrayValue();
  for (size_t i = 0; i < dbv.size(); ++i) {
    NValue val = dbv[i].GetValue();
    arr->Append(val->DeepCopy());
  }

  FastoObject* child = new FastoObject(out, arr, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Migrate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MIGRATE")}, argv), out);
}

common::Error CommandsApi::Move(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MOVE")}, argv), out);
}

common::Error CommandsApi::Mset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  std::vector<NDbKValue> keys;
  for (size_t i = 0; i < argv.size(); i += 2) {
    const nkey_t key_str(argv[i]);
    const command_buffer_t val_str = argv[i + 1];
    const NKey key(key_str);
    const NValue val(common::Value::CreateStringValue(val_str));
    const NDbKValue kv(key, val);
    keys.push_back(kv);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  common::Error err = redis->Mset(keys);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::MsetNX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  std::vector<NDbKValue> keys;
  for (size_t i = 0; i < argv.size(); i += 2) {
    const nkey_t key_str(argv[i]);
    const command_buffer_t val_str = argv[i + 1];
    const NKey key(key_str);
    const NValue val(common::Value::CreateStringValue(val_str));
    const NDbKValue kv(key, val);
    keys.push_back(kv);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long result = 0;
  common::Error err = redis->MsetNX(keys, &result);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(result);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Multi(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MULTI")}, argv), out);
}

common::Error CommandsApi::Object(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("OBJECT")}, argv), out);
}

common::Error CommandsApi::Pexpire(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  pttl_t ttl;
  if (!common::ConvertFromBytes(argv[1], &ttl)) {
    return common::make_error_inval();
  }

  common::Error err = red->PExpire(key, ttl);
  if (err) {
    common::FundamentalValue* val = common::Value::CreateUIntegerValue(0);
    FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
    out->AddChildren(child);
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(1);
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::PexpireAt(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PEXPIREAT")}, argv), out);
}

common::Error CommandsApi::Pfadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PFADD")}, argv), out);
}

common::Error CommandsApi::Pfcount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PFCOUNT")}, argv), out);
}

common::Error CommandsApi::Pfmerge(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PFMERGE")}, argv), out);
}

common::Error CommandsApi::Ping(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PING")}, argv), out);
}

common::Error CommandsApi::PsetEx(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PSETEX")}, argv), out);
}

common::Error CommandsApi::Pttl(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  pttl_t ttl;
  common::Error err = red->PTTL(key, &ttl);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(ttl);
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Publish(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(DB_PUBLISH_COMMAND)}, argv), out);
}

common::Error CommandsApi::PubSub(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PUBSUB")}, argv), out);
}

common::Error CommandsApi::PunSubscribe(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PUNSUBSCRIBE")}, argv), out);
}

common::Error CommandsApi::RandomKey(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RANDOMKEY")}, argv), out);
}

common::Error CommandsApi::ReadOnly(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("READONLY")}, argv), out);
}

common::Error CommandsApi::ReadWrite(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("READWRITE")}, argv), out);
}

common::Error CommandsApi::RenameNx(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RENAMENX")}, argv), out);
}

common::Error CommandsApi::Restore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RESTORE")}, argv), out);
}

common::Error CommandsApi::Role(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ROLE")}, argv), out);
}

common::Error CommandsApi::Rpop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RPOP")}, argv), out);
}

common::Error CommandsApi::RpopLpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RPOPLPUSH")}, argv), out);
}

common::Error CommandsApi::Rpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RPUSH")}, argv), out);
}

common::Error CommandsApi::RpushX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RPUSHX")}, argv), out);
}

common::Error CommandsApi::Save(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SAVE")}, argv), out);
}

common::Error CommandsApi::Scard(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SCARD")}, argv), out);
}

common::Error CommandsApi::ScriptDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SCRIPT"), GEN_CMD_STRING("DEBUG")}, argv),
                         out);
}

common::Error CommandsApi::ScriptExists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SCRIPT"), GEN_CMD_STRING("EXISTS")}, argv),
                         out);
}

common::Error CommandsApi::ScriptFlush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SCRIPT"), GEN_CMD_STRING("FLUSH")}, argv),
                         out);
}

common::Error CommandsApi::ScriptKill(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SCRIPT"), GEN_CMD_STRING("KILL")}, argv),
                         out);
}

common::Error CommandsApi::ScriptLoad(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SCRIPT"), GEN_CMD_STRING("LOAD")}, argv),
                         out);
}

common::Error CommandsApi::Sdiff(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SDIFF")}, argv), out);
}

common::Error CommandsApi::SdiffStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SDIFFSTORE")}, argv), out);
}

common::Error CommandsApi::SetBit(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SETBIT")}, argv), out);
}

common::Error CommandsApi::SetRange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SETRANGE")}, argv), out);
}

common::Error CommandsApi::Shutdown(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SHUTDOWN")}, argv), out);
}

common::Error CommandsApi::Sinter(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SINTER")}, argv), out);
}

common::Error CommandsApi::SinterStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SINTERSTORE")}, argv), out);
}

common::Error CommandsApi::SisMember(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SISMEMBER")}, argv), out);
}

common::Error CommandsApi::SlaveOf(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SLAVEOF")}, argv), out);
}

common::Error CommandsApi::SlowLog(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SLOWLOG")}, argv), out);
}

common::Error CommandsApi::Smove(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SMOVE")}, argv), out);
}

common::Error CommandsApi::Sort(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SORT")}, argv), out);
}

common::Error CommandsApi::Spop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SPOP")}, argv), out);
}

common::Error CommandsApi::SRandMember(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SRANDMEMBER")}, argv), out);
}

common::Error CommandsApi::Srem(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SREM")}, argv), out);
}

common::Error CommandsApi::Sscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SSCAN")}, argv), out);
}

common::Error CommandsApi::StrLen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("STRLEN")}, argv), out);
}

common::Error CommandsApi::Sunion(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SUNION")}, argv), out);
}

common::Error CommandsApi::SunionStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SUNIONSTORE")}, argv), out);
}

common::Error CommandsApi::Time(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("TIME")}, argv), out);
}

common::Error CommandsApi::Unsubscribe(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("UNSUBSCRIBE")}, argv), out);
}

common::Error CommandsApi::Unwatch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("UNWATCH")}, argv), out);
}

common::Error CommandsApi::Wait(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("WAIT")}, argv), out);
}

common::Error CommandsApi::Watch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("WATCH")}, argv), out);
}

common::Error CommandsApi::Zcard(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZCARD")}, argv), out);
}

common::Error CommandsApi::Zcount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZCOUNT")}, argv), out);
}

common::Error CommandsApi::ZincrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZINCRBY")}, argv), out);
}

common::Error CommandsApi::ZincrStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZINTERSTORE")}, argv), out);
}

common::Error CommandsApi::ZlexCount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZLEXCOUNT")}, argv), out);
}

common::Error CommandsApi::ZrangeByLex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZRANGEBYLEX")}, argv), out);
}

common::Error CommandsApi::ZrangeByScore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZRANGEBYSCORE")}, argv), out);
}

common::Error CommandsApi::Zrank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZRANK")}, argv), out);
}

common::Error CommandsApi::Zrem(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREM")}, argv), out);
}

common::Error CommandsApi::ZremRangeByLex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREMRANGEBYLEX")}, argv), out);
}

common::Error CommandsApi::ZremRangeByRank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREMRANGEBYRANK")}, argv), out);
}

common::Error CommandsApi::ZremRangeByScore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREMRANGEBYSCORE")}, argv), out);
}

common::Error CommandsApi::ZrevRange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREVRANGE")}, argv), out);
}

common::Error CommandsApi::ZrevRangeByLex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREVRANGEBYLEX")}, argv), out);
}

common::Error CommandsApi::ZrevRangeByScore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREVRANGEBYSCORE")}, argv), out);
}

common::Error CommandsApi::ZrevRank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZREVRANK")}, argv), out);
}

common::Error CommandsApi::Zscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZSCAN")}, argv), out);
}

common::Error CommandsApi::Zscore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZSCORE")}, argv), out);
}

common::Error CommandsApi::ZunionStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ZUNIONSTORE")}, argv), out);
}

common::Error CommandsApi::SentinelMasters(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("MASTERS")}, argv),
                         out);
}

common::Error CommandsApi::SentinelMaster(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("MASTER")}, argv),
                         out);
}

common::Error CommandsApi::SentinelSlaves(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("SLAVES")}, argv),
                         out);
}

common::Error CommandsApi::SentinelSentinels(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("SENTINELS")}, argv), out);
}

common::Error CommandsApi::SentinelGetMasterAddrByName(internal::CommandHandler* handler,
                                                       commands_args_t argv,
                                                       FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("GET-MASTER-ADDR-BY-NAME")}, argv),
      out);
}

common::Error CommandsApi::SentinelReset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("RESET")}, argv),
                         out);
}

common::Error CommandsApi::SentinelFailover(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("FAILOVER")}, argv), out);
}

common::Error CommandsApi::SentinelCkquorum(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("CKQUORUM")}, argv), out);
}

common::Error CommandsApi::SentinelFlushConfig(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("FLUSHCONFIG")}, argv), out);
}

common::Error CommandsApi::SentinelMonitor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("MONITOR")}, argv),
                         out);
}

common::Error CommandsApi::SentinelRemove(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("REMOVE")}, argv),
                         out);
}

common::Error CommandsApi::SentinelSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SENTINEL"), GEN_CMD_STRING("SET")}, argv),
                         out);
}

common::Error CommandsApi::Monitor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  argv.push_front(GEN_CMD_STRING("MONITOR"));
  return red->Monitor(argv, out);
}

common::Error CommandsApi::Subscribe(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  argv.push_front(GEN_CMD_STRING(DB_SUBSCRIBE_COMMAND));
  return red->Subscribe(argv, out);
}

common::Error CommandsApi::Sync(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  UNUSED(argv);
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->SlaveMode(out);
}

}  // namespace pika
}  // namespace core
}  // namespace fastonosql
