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

#include "core/db/keydb/internal/commands_api.h"

#include <string>
#include <vector>

#include <common/string_util.h>

#include <fastonosql/core/db/keydb/db_connection.h>
#include "core/db/keydb/internal/modules.h"

#include <fastonosql/core/value.h>

namespace fastonosql {
namespace core {
namespace keydb {

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
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  pttl_t ttl;
  if (!common::ConvertFromBytes(argv[1], &ttl)) {
    return common::make_error_inval();
  }

  DBConnection* red = static_cast<DBConnection*>(handler);
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
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  pttl_t ttl;
  DBConnection* red = static_cast<DBConnection*>(handler);
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
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  common::ArrayValue* arr = common::Value::CreateArrayValue();
  for (size_t i = 1; i < argv.size(); ++i) {
    arr->AppendString(argv[i]);
  }

  DBConnection* redis = static_cast<DBConnection*>(handler);
  long long list_len = 0;
  common::Error err = redis->Rpush(key, NValue(arr), &list_len);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(list_len);
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
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

common::Error CommandsApi::ReplicaOf(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("REPLICAOF")}, argv), out);
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

#if defined(PRO_VERSION)
common::Error CommandsApi::ModuleLoad(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  ModuleInfo mod;
  mod.name = common::ConvertToString(argv[0]);

  common::Error err = red->ModuleLoad(mod);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::ModuleUnLoad(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  ModuleInfo mod;
  mod.name = common::ConvertToString(argv[0]);

  common::Error err = red->ModuleUnLoad(mod);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}
#endif

// extend comands
common::Error CommandsApi::Latency(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LATENCY")}, argv), out);
}

common::Error CommandsApi::PFDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PFDEBUG")}, argv), out);
}

common::Error CommandsApi::ReplConf(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("REPLCONF")}, argv), out);
}

common::Error CommandsApi::Substr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SUBSTR")}, argv), out);
}

#if defined(PRO_VERSION)
common::Error CommandsApi::ModuleList(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MODULE"), GEN_CMD_STRING("LIST")}, argv),
                         out);
}
#endif

common::Error CommandsApi::MemoryDoctor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MEMORY"), GEN_CMD_STRING("DOCTOR")}, argv),
                         out);
}

common::Error CommandsApi::MemoryUsage(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MEMORY"), GEN_CMD_STRING("USAGE")}, argv),
                         out);
}

common::Error CommandsApi::MemoryStats(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MEMORY"), GEN_CMD_STRING("STATS")}, argv),
                         out);
}

common::Error CommandsApi::MemoryPurge(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("MEMORY"), GEN_CMD_STRING("PURGE")}, argv),
                         out);
}

common::Error CommandsApi::MemoryMallocStats(internal::CommandHandler* handler,
                                             commands_args_t argv,
                                             FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING("MEMORY"), GEN_CMD_STRING("MALLOC-STATS")}, argv), out);
}

common::Error CommandsApi::SwapDB(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("SWAPDB")}, argv), out);
}

common::Error CommandsApi::Unlink(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  NKeys keysdel;
  for (size_t i = 0; i < argv.size(); ++i) {
    const nkey_t key_str(argv[i]);
    const NKey key(key_str);

    keysdel.push_back(key);
  }

  DBConnection* red = static_cast<DBConnection*>(handler);
  NKeys keys_deleted;
  common::Error err = red->Unlink(keysdel, &keys_deleted);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateUIntegerValue(keys_deleted.size());
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Touch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("TOUCH")}, argv), out);
}

common::Error CommandsApi::Xlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XLEN")}, argv), out);
}

common::Error CommandsApi::Xrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* red = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = red->XRange(key, &key_loaded, out);
  if (err) {
    return err;
  }
  return common::Error();
}

common::Error CommandsApi::Xrevrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XREVRANGE")}, argv), out);
}

common::Error CommandsApi::Xread(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XREAD")}, argv), out);
}

common::Error CommandsApi::Xadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  StreamValue::stream_id sid = argv[1];
  StreamValue* stream = new StreamValue;
  std::vector<StreamValue::Entry> entr;
  for (size_t i = 2; i < argv.size(); i += 2) {
    command_buffer_t key = argv[i];
    command_buffer_t val = argv[i + 1];
    entr.push_back(StreamValue::Entry{key, val});
  }
  stream->SetStreams({StreamValue::Stream{sid, entr}});
  NDbKValue kv(key, NValue(stream));

  DBConnection* red = static_cast<DBConnection*>(handler);
  StreamValue::stream_id gen_id;
  common::Error err = red->XAdd(kv, &gen_id);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(gen_id);
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::XFastoSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  if (argv.size() == 1 || argv.size() % 3 != 1) {
    return common::make_error_inval();
  }

  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  StreamValue* stream = new StreamValue;
  StreamValue::streams_t streams;
  for (size_t i = 1; i < argv.size(); i += 3) {
    StreamValue::stream_id sid = argv[i];
    StreamValue::Entry ent = {argv[i + 1], argv[i + 2]};

    bool is_found = false;
    for (size_t j = 0; j < streams.size(); ++j) {
      if (streams[j].sid == sid) {
        streams[j].entries.push_back(ent);
        is_found = true;
        break;
      }
    }

    if (!is_found) {
      streams.push_back({sid, {ent}});
    }
  }
  stream->SetStreams(streams);

  DBConnection* redis = static_cast<DBConnection*>(handler);
  common::Error err = redis->XFastoSet(key, NValue(stream));
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::Xack(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XACK")}, argv), out);
}

common::Error CommandsApi::XreadGroup(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XREADGROUP")}, argv), out);
}

common::Error CommandsApi::XGroup(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XGROUP")}, argv), out);
}

common::Error CommandsApi::Xclaim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XCLAIM")}, argv), out);
}

common::Error CommandsApi::XTrim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XTRIM")}, argv), out);
}

common::Error CommandsApi::XPending(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XPENDING")}, argv), out);
}

common::Error CommandsApi::Xinfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XINFO")}, argv), out);
}

common::Error CommandsApi::Xdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XDEL")}, argv), out);
}

common::Error CommandsApi::BZpopMin(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BZPOPMIN")}, argv), out);
}

common::Error CommandsApi::BZpopMax(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("BZPOPMAX")}, argv), out);
}

common::Error CommandsApi::ZpopMin(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  CHECK_LE(argv.size(), 2) << "ZpopMin <key> [count].";
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);
  size_t count = 1;
  if (argv.size() == 2 && !common::ConvertFromBytes(argv[1], &count)) {
    return common::make_error_inval();
  }

  DBConnection* red = static_cast<DBConnection*>(handler);
  common::ArrayValue* arr = nullptr;
  common::Error err = red->ZpopMin(key, count, &arr);
  if (err) {
    return err;
  }

  FastoObject* child = new FastoObject(out, arr, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::ZpopMax(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  CHECK_LE(argv.size(), 2) << "ZpopMax <key> [count].";
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);
  size_t count = 1;
  if (argv.size() == 2 && !common::ConvertFromBytes(argv[1], &count)) {
    return common::make_error_inval();
  }

  DBConnection* red = static_cast<DBConnection*>(handler);
  common::ArrayValue* arr = nullptr;
  common::Error err = red->ZpopMax(key, count, &arr);
  if (err) {
    return err;
  }

  FastoObject* child = new FastoObject(out, arr, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::PFSelfTest(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("PFSELFTEST")}, argv), out);
}

common::Error CommandsApi::XSetID(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("XSETID")}, argv), out);
}

common::Error CommandsApi::LolWut(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("LOLWUT")}, argv), out);
}

common::Error CommandsApi::Hello(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("HELLO")}, argv), out);
}

common::Error CommandsApi::Acl(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ACL")}, argv), out);
}

common::Error CommandsApi::Asking(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("ASKING")}, argv), out);
}

common::Error CommandsApi::RestoreAsking(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("RESTORE-ASKING")}, argv), out);
}

common::Error CommandsApi::GeoRadius_ro(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEORADIUS_RO")}, argv), out);
}

common::Error CommandsApi::GeoRadiusByMember_ro(internal::CommandHandler* handler,
                                                commands_args_t argv,
                                                FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING("GEORADIUSBYMEMBER_RO")}, argv), out);
}

// modules
common::Error CommandsApi::GraphQuery(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->GraphQuery(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_GRAPH_MODULE_COMMAND("QUERY"))}, argv),
                         out);
}

common::Error CommandsApi::GraphExplain(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->GraphExplain(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_GRAPH_MODULE_COMMAND("EXPLAIN"))}, argv), out);
}

common::Error CommandsApi::GraphDelete(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->GraphDelete(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_GRAPH_MODULE_COMMAND("DELETE"))}, argv),
                          out);
}

common::Error CommandsApi::FtAdd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("ADD"))}, argv),
                         out);
}

common::Error CommandsApi::FtCreate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("CREATE"))}, argv),
                         out);
}

common::Error CommandsApi::FtSearch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SEARCH"))}, argv),
                         out);
}

common::Error CommandsApi::FtAddHash(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("ADDHASH"))}, argv), out);
}

common::Error CommandsApi::FtInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("INFO"))}, argv),
                         out);
}

common::Error CommandsApi::FtOptimize(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("OPTIMIZE"))}, argv), out);
}

common::Error CommandsApi::FtExplain(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("EXPLAIN"))}, argv), out);
}

common::Error CommandsApi::FtDel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DEL"))}, argv),
                         out);
}

common::Error CommandsApi::FtGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("GET"))}, argv),
                         out);
}

common::Error CommandsApi::FtMGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("MGET"))}, argv),
                         out);
}

common::Error CommandsApi::FtDrop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DROP"))}, argv),
                         out);
}

common::Error CommandsApi::FtSugadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SUGGADD"))}, argv), out);
}

common::Error CommandsApi::FtSugget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SUGGET"))}, argv),
                         out);
}

common::Error CommandsApi::FtSugdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SUGDEL"))}, argv),
                         out);
}

common::Error CommandsApi::FtSuglen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SUGLEN"))}, argv),
                         out);
}

common::Error CommandsApi::FtConfig(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("CONFIG"))}, argv),
                         out);
}

common::Error CommandsApi::FtSynupdate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SYNUPDATE"))}, argv), out);
}

common::Error CommandsApi::FtAlter(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("ALTER"))}, argv),
                         out);
}

common::Error CommandsApi::FtDictdump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DICTDUMP"))}, argv), out);
}

common::Error CommandsApi::FtDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DEBUG"))}, argv),
                         out);
}

common::Error CommandsApi::FtDictdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DICTDEL"))}, argv), out);
}

common::Error CommandsApi::FtAggregate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("AGGREGATE"))}, argv), out);
}

common::Error CommandsApi::FtSynforceupdate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SYNFORCEUPDATE"))}, argv), out);
}
common::Error CommandsApi::FtExplaincli(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("EXPLAINCLI"))}, argv), out);
}

common::Error CommandsApi::FtSpellcheck(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SPELLCHECK"))}, argv), out);
}

common::Error CommandsApi::FtSyndump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SYNDUMP"))}, argv), out);
}

common::Error CommandsApi::FtSynadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("SYNADD"))}, argv),
                         out);
}

common::Error CommandsApi::FtDictadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("DICTADD"))}, argv), out);
}

common::Error CommandsApi::FtTagvals(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("TAGVALS"))}, argv), out);
}

common::Error CommandsApi::FtCursor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_SEARCH_MODULE_COMMAND("CURSOR"))}, argv),
                         out);
}

common::Error CommandsApi::JsonDel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* cdb = static_cast<DBConnection*>(handler);
  long long deleted;
  common::Error err = cdb->JsonDel(key, &deleted);
  if (err) {
    return err;
  }

  common::FundamentalValue* val = common::Value::CreateLongLongIntegerValue(deleted);
  FastoObject* child = new FastoObject(out, val, cdb->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::JsonGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  DBConnection* red = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = red->JsonGet(key, &key_loaded);
  if (err) {
    return err;
  }

  NValue val = key_loaded.GetValue();
  common::Value* copy = val->DeepCopy();
  FastoObject* child = new FastoObject(out, copy, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::JsonMget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("MGET"))}, argv),
                         out);
}

common::Error CommandsApi::JsonSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const nkey_t key_str(argv[0]);
  const NKey key(key_str);

  NValue json_val(new JsonValue(argv[2]));
  NDbKValue kv(key, json_val);

  DBConnection* red = static_cast<DBConnection*>(handler);
  common::Error err = red->JsonSet(kv);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, red->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error CommandsApi::JsonType(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("TYPE"))}, argv),
                         out);
}

common::Error CommandsApi::JsonNumIncrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("NUMINCRBY"))}, argv), out);
}

common::Error CommandsApi::JsonNumMultBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("NUMMULTBY"))}, argv), out);
}

common::Error CommandsApi::JsonStrAppend(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("STRAPPEND"))}, argv), out);
}

common::Error CommandsApi::JsonStrlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("STRLEN"))}, argv),
                         out);
}

common::Error CommandsApi::JsonArrAppend(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("ARRAPPEND"))}, argv), out);
}

common::Error CommandsApi::JsonArrIndex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("ARRINDEX"))}, argv),
                         out);
}

common::Error CommandsApi::JsonArrInsert(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("ARRINSERT"))}, argv), out);
}

common::Error CommandsApi::JsonArrLen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("ARRLEN"))}, argv),
                         out);
}

common::Error CommandsApi::JsonArrPop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("ARRPOP"))}, argv),
                         out);
}

common::Error CommandsApi::JsonArrTrim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("ARRTRIM"))}, argv),
                         out);
}

common::Error CommandsApi::JsonObjKeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("OBJKEYS"))}, argv),
                         out);
}

common::Error CommandsApi::JsonObjLen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("OBJLEN"))}, argv),
                         out);
}

common::Error CommandsApi::JsonDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("DEBUG"))}, argv),
                         out);
}

common::Error CommandsApi::JsonForget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("FORGET"))}, argv),
                         out);
}

common::Error CommandsApi::JsonResp(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_JSON_MODULE_COMMAND("RESP"))}, argv),
                         out);
}

common::Error CommandsApi::NrReset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("RESET"))}, argv),
                         out);
}

common::Error CommandsApi::NrInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("INFO"))}, argv), out);
}

common::Error CommandsApi::NrGetData(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("GETDATA"))}, argv),
                         out);
}

common::Error CommandsApi::NrRun(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("RUN"))}, argv), out);
}

common::Error CommandsApi::NrClass(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("CLASS"))}, argv),
                         out);
}

common::Error CommandsApi::NrCreate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("CREATE"))}, argv),
                         out);
}

common::Error CommandsApi::NrObserve(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("OBSERVE"))}, argv),
                         out);
}

common::Error CommandsApi::NrTrain(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("TRAIN"))}, argv),
                         out);
}

common::Error CommandsApi::NrThreads(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_NR_MODULE_COMMAND("THREADS"))}, argv),
                         out);
}

common::Error CommandsApi::BfDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("DEBUG"))}, argv),
                         out);
}

common::Error CommandsApi::BfExists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("EXISTS"))}, argv),
                         out);
}

common::Error CommandsApi::BfScanDump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("SCANDUMP"))}, argv), out);
}

common::Error CommandsApi::BfAdd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("ADD"))}, argv),
                         out);
}

common::Error CommandsApi::BfMexists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("MEXISTS"))}, argv),
                         out);
}

common::Error CommandsApi::BfLoadChunk(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(
      redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("LOADCHUNK"))}, argv), out);
}

common::Error CommandsApi::BfMadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("MADD"))}, argv),
                         out);
}

common::Error CommandsApi::BfReserve(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  DBConnection* red = static_cast<DBConnection*>(handler);
  return red->CommonExec(redis_compatible::ExpandCommand({GEN_CMD_STRING(REDIS_BLOOM_MODULE_COMMAND("RESERVE"))}, argv),
                         out);
}

common::Error CommandsApi::ClientSetName(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out) {
  const std::string name = argv[0].as_string();

  DBConnection* redis = static_cast<DBConnection*>(handler);
  common::Error err = redis->SetClientName(name);
  if (err) {
    return err;
  }

  common::StringValue* val = common::Value::CreateStringValue(GEN_CMD_STRING(OK_RESULT));
  FastoObject* child = new FastoObject(out, val, redis->GetDelimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace keydb
}  // namespace core
}  // namespace fastonosql
