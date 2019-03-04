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

#pragma once

#include "core/db/redis_compatible/internal/commands_api.h"

namespace fastonosql {
namespace core {
namespace redis {

class DBConnection;
struct CommandsApi : public redis_compatible::CommandsApi<DBConnection> {
  static common::Error BgRewriteAof(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BgSave(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BitCount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BitField(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BitOp(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BitPos(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BlPop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BrPop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BrPopLpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  static common::Error ClientGetName(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClientSetName(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClientKill(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClientList(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClientPause(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClientReply(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  static common::Error ClusterAddSlots(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterCountFailureReports(internal::CommandHandler* handler,
                                                  commands_args_t argv,
                                                  FastoObject* out);
  static common::Error ClusterCountKeysSinSlot(internal::CommandHandler* handler,
                                               commands_args_t argv,
                                               FastoObject* out);
  static common::Error ClusterDelSlots(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterFailover(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterForget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterGetKeySinSlot(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterKeySlot(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterMeet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterNodes(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterReplicate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterReset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterSaveConfig(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterSetConfigEpoch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterSetSlot(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterSlaves(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ClusterSlots(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error CommandCount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error CommandGetKeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error CommandInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Command(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ConfigGet(internal::CommandHandler* handler,
                                 commands_args_t argv,
                                 FastoObject* out);  // CONFIG GET works differently than in redis
                                                     // protocol (only databases)
  static common::Error ConfigResetStat(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ConfigRewrite(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ConfigSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error DebugObject(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error DebugSegFault(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Discard(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Dump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Echo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Eval(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error EvalSha(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Exec(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Exists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ExpireAt(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FlushALL(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoAdd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoDist(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoHash(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoPos(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoRadius(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoRadiusByMember(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GetBit(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GetRange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GetSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hexists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error HincrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error HincrByFloat(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hkeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hmget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error HsetNX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hstrlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Hvals(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error RKeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error LastSave(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Lindex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Linsert(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Llen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Lpop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error LpushX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Lrem(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Lset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Ltrim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Mget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Migrate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Move(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Mset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error MsetNX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Multi(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Object(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Pexpire(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error PexpireAt(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Pfadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Pfcount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Pfmerge(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Ping(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error PsetEx(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Pttl(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Publish(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error PubSub(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error PunSubscribe(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error RandomKey(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ReadOnly(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ReadWrite(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error RenameNx(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Restore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Role(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Rpop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error RpopLpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Rpush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error RpushX(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Save(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Scard(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ScriptDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ScriptExists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ScriptFlush(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ScriptKill(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ScriptLoad(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Sdiff(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SdiffStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SetBit(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SetRange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Shutdown(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Sinter(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SinterStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SisMember(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SlaveOf(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ReplicaOf(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SlowLog(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Smove(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Sort(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Spop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SRandMember(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Srem(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Sscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error StrLen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Sunion(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SunionStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Time(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Unsubscribe(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Unwatch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Wait(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Watch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zcard(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zcount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZincrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZincrStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZlexCount(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZrangeByLex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZrangeByScore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zrank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zrem(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZremRangeByLex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZremRangeByRank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZremRangeByScore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZrevRange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZrevRangeByLex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZrevRangeByScore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZrevRank(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zscan(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Zscore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZunionStore(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelMasters(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelMaster(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelSlaves(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelSentinels(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelGetMasterAddrByName(internal::CommandHandler* handler,
                                                   commands_args_t argv,
                                                   FastoObject* out);
  static common::Error SentinelReset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelFailover(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelCkquorum(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelFlushConfig(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelMonitor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelRemove(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SentinelSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  static common::Error Monitor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Subscribe(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Sync(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

#if defined(PRO_VERSION)
  static common::Error ModuleLoad(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ModuleUnLoad(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
#endif
  // extend commands
  static common::Error Latency(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error PFDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ReplConf(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Substr(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error PFSelfTest(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error XSetID(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error LolWut(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
#if defined(PRO_VERSION)
  static common::Error ModuleList(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
#endif
  static common::Error MemoryDoctor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error MemoryUsage(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error MemoryStats(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error MemoryPurge(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error MemoryMallocStats(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error SwapDB(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Unlink(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Touch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xrevrange(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xread(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error XFastoSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xack(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error XreadGroup(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error XGroup(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xclaim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error XTrim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error XPending(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xinfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Xdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BZpopMin(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BZpopMax(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZpopMin(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error ZpopMax(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error Asking(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error RestoreAsking(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoRadius_ro(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GeoRadiusByMember_ro(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // redis-graph api
  static common::Error GraphQuery(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GraphExplain(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error GraphDelete(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // redisearch
  static common::Error FtAdd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtCreate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSearch(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtAddHash(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtOptimize(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtExplain(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtDel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtMGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtDrop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSugadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSugget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSugdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSuglen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtConfig(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSynupdate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtAlter(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtDictdump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtDictdel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtAggregate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSynforceupdate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtExplaincli(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSpellcheck(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSyndump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtSynadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtDictadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtTagvals(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error FtCursor(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // rejson
  static common::Error JsonDel(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonGet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonMget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonSet(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonType(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonNumIncrBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonNumMultBy(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonStrAppend(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonStrlen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonArrAppend(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonArrIndex(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonArrInsert(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonArrLen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonArrPop(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonArrTrim(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonObjKeys(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonObjLen(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonForget(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error JsonResp(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // nr
  static common::Error NrReset(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrInfo(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrGetData(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrRun(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrClass(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrCreate(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrObserve(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrTrain(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error NrThreads(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);

  // bf
  static common::Error BfDebug(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfExists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfScanDump(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfAdd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfMexists(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfLoadChunk(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfMadd(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
  static common::Error BfReserve(internal::CommandHandler* handler, commands_args_t argv, FastoObject* out);
};

}  // namespace redis
}  // namespace core
}  // namespace fastonosql
