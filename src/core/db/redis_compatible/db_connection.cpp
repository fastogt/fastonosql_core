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

#include <fastonosql/core/db/redis_compatible/db_connection.h>

extern "C" {
#include <hiredis/hiredis.h>
}

#include <common/file_system/string_path_utils.h>

#if defined(PRO_VERSION)
#include <fastonosql/core/db/redis_compatible/cluster_infos.h>
#include <fastonosql/core/db/redis_compatible/sentinel_info.h>
#endif

#include <fastonosql/core/db/redis_compatible/command_translator.h>
#include <fastonosql/core/db/redis_compatible/database_info.h>

#include <fastonosql/core/value.h>

#define GET_SERVER_TYPE "CLUSTER NODES"
#define GET_SENTINEL_MASTERS "SENTINEL MASTERS"
#define GET_SENTINEL_SLAVES_PATTERN_1ARGS_S "SENTINEL SLAVES %s"

#define DBSIZE "DBSIZE"

#define HIREDIS_VERSION    \
  STRINGIZE(HIREDIS_MAJOR) \
  "." STRINGIZE(HIREDIS_MINOR) "." STRINGIZE(HIREDIS_PATCH)

namespace fastonosql {
namespace core {
namespace redis_compatible {

bool ConvertFromString(const readable_string_t& value, common::Value::Type* out) {
  if (value.empty() || !out) {
    return false;
  }

  common::Value::Type lout = common::Value::TYPE_NULL;
  if (value == GEN_CMD_STRING("string")) {
    lout = common::Value::TYPE_STRING;
  } else if (value == GEN_CMD_STRING("list")) {
    lout = common::Value::TYPE_ARRAY;
  } else if (value == GEN_CMD_STRING("set")) {
    lout = common::Value::TYPE_SET;
  } else if (value == GEN_CMD_STRING("hash")) {
    lout = common::Value::TYPE_HASH;
  } else if (value == GEN_CMD_STRING("zset")) {
    lout = common::Value::TYPE_ZSET;
  } else if (value == GEN_CMD_STRING("stream")) {
    lout = core::StreamValue::TYPE_STREAM;
  } else if (value == GEN_CMD_STRING("ReJSON-RL")) {
    lout = core::JsonValue::TYPE_JSON;
  } else if (value == GEN_CMD_STRING("raxtype01")) {
    lout = core::GraphValue::TYPE_GRAPH;
  } else if (value == GEN_CMD_STRING("MBbloom--")) {
    lout = core::BloomValue::TYPE_BLOOM;
  } else if (value == GEN_CMD_STRING("ft_invidx")) {
    lout = core::SearchValue::TYPE_FT_TERM;
  } else if (value == GEN_CMD_STRING("ft_index0")) {
    lout = core::SearchValue::TYPE_FT_INDEX;
  }

  *out = lout;
  return true;
}

bool ConvertFromType(common::Value::Type type, readable_string_t* out) {
  if (!out) {
    return false;
  }

  readable_string_t lout = GEN_CMD_STRING("nil");
  if (type == common::Value::TYPE_STRING) {
    lout = GEN_CMD_STRING("string");
  } else if (type == common::Value::TYPE_ARRAY) {
    lout = GEN_CMD_STRING("list");
  } else if (type == common::Value::TYPE_SET) {
    lout = GEN_CMD_STRING("set");
  } else if (type == common::Value::TYPE_HASH) {
    lout = GEN_CMD_STRING("hash");
  } else if (type == common::Value::TYPE_ZSET) {
    lout = GEN_CMD_STRING("zset");
  } else if (type == core::StreamValue::TYPE_STREAM) {
    lout = GEN_CMD_STRING("stream");
  } else if (type == core::JsonValue::TYPE_JSON) {
    lout = GEN_CMD_STRING("ReJSON-RL");
  } else if (type == core::GraphValue::TYPE_GRAPH) {
    lout = GEN_CMD_STRING("raxtype01");
  } else if (type == core::BloomValue::TYPE_BLOOM) {
    lout = GEN_CMD_STRING("MBbloom--");
  } else if (type == core::SearchValue::TYPE_FT_TERM) {
    lout = GEN_CMD_STRING("ft_invidx");
  } else if (type == core::SearchValue::TYPE_FT_INDEX) {
    lout = GEN_CMD_STRING("ft_index0");
  }

  *out = lout;
  return true;
}

namespace {
bool IsPipeLineCommand(const command_buffer_t& command) {
  if (command.empty()) {
    DNOTREACHED();
    return false;
  }

  bool skip = strncasecmp(command.data(), "quit", command.size()) == 0 ||
              strncasecmp(command.data(), "exit", command.size()) == 0 ||
              strncasecmp(command.data(), "connect", command.size()) == 0 ||
              strncasecmp(command.data(), "help", command.size()) == 0 ||
              strncasecmp(command.data(), "?", command.size()) == 0 ||
              strncasecmp(command.data(), "shutdown", command.size()) == 0 ||
              strncasecmp(command.data(), "monitor", command.size()) == 0 ||
              strncasecmp(command.data(), "subscribe", command.size()) == 0 ||
              strncasecmp(command.data(), "psubscribe", command.size()) == 0 ||
              strncasecmp(command.data(), "sync", command.size()) == 0 ||
              strncasecmp(command.data(), "psync", command.size()) == 0;

  return !skip;
}
}  // namespace

const char* GetHiredisVersion() {
  return HIREDIS_VERSION;
}

common::Error CreateConnection(const Config& config, const SSHInfo& sinfo, NativeConnection** context) {
  if (!context) {
    return common::make_error_inval();
  }

  redisContext* lcontext = nullptr;
  bool is_local = !config.hostsocket.empty();

  if (is_local) {
    const char* hostsocket = config.hostsocket.empty() ? nullptr : config.hostsocket.c_str();
    lcontext = redisConnectUnix(hostsocket);
  } else {
    std::string host_str = config.host.GetHost();
    const char* host = host_str.empty() ? nullptr : host_str.c_str();
    bool is_ssl = config.is_ssl;
    uint16_t port = config.host.GetPort();
    std::string username_str = sinfo.GetUsername();
    const char* username = username_str.empty() ? nullptr : username_str.c_str();
    std::string runtime_password = sinfo.GetRuntimePassword();
    const char* password = runtime_password.empty() ? nullptr : runtime_password.c_str();
    common::net::HostAndPort ssh_host = sinfo.GetHost();
    std::string ssh_host_str = ssh_host.GetHost();
    const char* ssh_address = ssh_host_str.empty() ? nullptr : ssh_host_str.c_str();
    int ssh_port = ssh_host.GetPort();
    SSHInfo::AuthenticationMethod ssh_method = sinfo.GetAuthMethod();
    PublicPrivate key = sinfo.GetKey();
    const char* public_key = key.public_key_path.empty() ? nullptr : key.public_key_path.c_str();
    const char* private_key = key.private_key_path.empty() ? nullptr : key.private_key_path.c_str();
    bool use_public_key = key.use_public_key;
    std::string passphrase_str = sinfo.GetPassPharse();
    const char* passphrase = passphrase_str.empty() ? nullptr : passphrase_str.c_str();
    int rssh_method = SSH_UNKNOWN;
    if (ssh_method == SSHInfo::PUBLICKEY) {
      if (!private_key || !common::file_system::is_file_exist(private_key)) {
        return common::make_error(common::MemSPrintf("Invalid input private_key path: (%s).", private_key));
      }

      if (use_public_key) {
        if (!public_key || !common::file_system::is_file_exist(public_key)) {
          return common::make_error(common::MemSPrintf("Invalid input public_key path: (%s).", public_key));
        }
      }
      rssh_method = SSH_PUBLICKEY;
    } else if (ssh_method == SSHInfo::ASK_PASSWORD) {
      rssh_method = SSH_PASSWORD;
    } else if (ssh_method == SSHInfo::PASSWORD) {
      rssh_method = SSH_PASSWORD;
    }
    lcontext = redisConnectSSH(host, port, ssh_address, ssh_port, username, password, public_key, private_key,
                               passphrase, is_ssl, rssh_method);
  }

  if (!lcontext) {
    if (is_local) {
      return common::make_error(common::MemSPrintf("Could not connect to Redis at %s : no context", config.hostsocket));
    }
    std::string host_str = common::ConvertToString(config.host);
    return common::make_error(common::MemSPrintf("Could not connect to Redis at %s : no context", host_str));
  }

  if (lcontext->err) {
    std::string buff;
    if (is_local) {
      buff = common::MemSPrintf("Could not connect to Redis at %s : %s", config.hostsocket, lcontext->errstr);
    } else {
      std::string host_str = common::ConvertToString(config.host);
      buff = common::MemSPrintf("Could not connect to Redis at %s : %s", host_str, lcontext->errstr);
    }
    redisFree(lcontext);
    return common::make_error(buff);
  }

  *context = lcontext;
  return common::Error();
}

common::Error TestConnection(const Config& config, const SSHInfo& sinfo) {
  redisContext* context = nullptr;
  common::Error err = CreateConnection(config, sinfo, &context);
  if (err) {
    return err;
  }

  err = AuthContext(context, common::ConvertToCharBytes(config.auth));  // convert from std::string to char bytes
  if (err) {
    redisFree(context);
    return err;
  }

  redisFree(context);
  return common::Error();
}

#if defined(PRO_VERSION)
common::Error DiscoveryClusterConnection(const Config& rconfig,
                                         const SSHInfo& sinfo,
                                         std::vector<ServerDiscoveryClusterInfoSPtr>* infos) {
  if (!infos) {
    return common::make_error_inval();
  }

  redisContext* context = nullptr;
  common::Error err = CreateConnection(rconfig, sinfo, &context);
  if (err) {
    return err;
  }

  err = AuthContext(context, common::ConvertToCharBytes(rconfig.auth));  // convert from std::string to char bytes
  if (err) {
    redisFree(context);
    return err;
  }

  /* Send the GET CLUSTER command. */
  redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(context, GET_SERVER_TYPE));
  if (!reply) {
    err = common::make_error("I/O error");
    redisFree(context);
    return err;
  }

  if (reply->type == REDIS_REPLY_STRING) {
    err = MakeDiscoveryClusterInfo(rconfig.host, std::string(reply->str, reply->len), infos);
  } else if (reply->type == REDIS_REPLY_ERROR) {
    err = common::make_error(std::string(reply->str, reply->len));
  } else {
    DNOTREACHED();
  }

  freeReplyObject(reply);
  redisFree(context);
  return err;
}

common::Error DiscoverySentinelConnection(const Config& rconfig,
                                          const SSHInfo& sinfo,
                                          std::vector<ServerDiscoverySentinelInfoSPtr>* infos) {
  if (!infos) {
    return common::make_error_inval();
  }

  redisContext* context = nullptr;
  common::Error err = CreateConnection(rconfig, sinfo, &context);
  if (err) {
    return err;
  }

  err = AuthContext(context, common::ConvertToCharBytes(rconfig.auth));  // convert from std::string to char bytes
  if (err) {
    redisFree(context);
    return err;
  }

  /* Send the GET MASTERS command. */
  redisReply* masters_reply = reinterpret_cast<redisReply*>(redisCommand(context, GET_SENTINEL_MASTERS));
  if (!masters_reply) {
    redisFree(context);
    return common::make_error("I/O error");
  }

  for (size_t i = 0; i < masters_reply->elements; ++i) {
    redisReply* master_info = masters_reply->element[i];
    ServerCommonInfo sinf;
    common::Error lerr = MakeServerCommonInfo(master_info, &sinf);
    if (lerr) {
      continue;
    }

    const char* master_name = sinf.name.c_str();
    ServerDiscoverySentinelInfoSPtr sent(new DiscoverySentinelInfo(sinf));
    infos->push_back(sent);
    /* Send the GET SLAVES command. */
    redisReply* reply =
        reinterpret_cast<redisReply*>(redisCommand(context, GET_SENTINEL_SLAVES_PATTERN_1ARGS_S, master_name));
    if (!reply) {
      freeReplyObject(masters_reply);
      redisFree(context);
      return common::make_error("I/O error");
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
      for (size_t j = 0; j < reply->elements; ++j) {
        redisReply* server_info = reply->element[j];
        ServerCommonInfo slsinf;
        lerr = MakeServerCommonInfo(server_info, &slsinf);
        if (lerr) {
          continue;
        }
        ServerDiscoverySentinelInfoSPtr lsent(new DiscoverySentinelInfo(slsinf));
        infos->push_back(lsent);
      }
    } else if (reply->type == REDIS_REPLY_ERROR) {
      freeReplyObject(reply);
      freeReplyObject(masters_reply);
      redisFree(context);
      return common::make_error(std::string(reply->str, reply->len));
    } else {
      DNOTREACHED();
    }
    freeReplyObject(reply);
  }

  freeReplyObject(masters_reply);
  redisFree(context);
  return common::Error();
}
#endif

common::Error PrintRedisContextError(NativeConnection* context) {
  if (!context) {
    DNOTREACHED();
    return common::make_error("Not connected");
  }

  return common::make_error(common::MemSPrintf("Error: %s", context->errstr));
}

common::Error ValueFromReplay(redisReply* r, common::Value** out) {
  if (!out || !r) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  switch (r->type) {
    case REDIS_REPLY_NIL: {
      *out = common::Value::CreateNullValue();
      break;
    }
    case REDIS_REPLY_ERROR: {
      if (common::strcasestr(r->str, "NOAUTH")) {  // "NOAUTH Authentication
                                                   // required."
      }
      std::string str(r->str, r->len);
      return common::make_error(str);
    }
    case REDIS_REPLY_STATUS:
    case REDIS_REPLY_STRING: {
      *out = common::Value::CreateStringValue(GEN_CMD_STRING_SIZE(r->str, r->len));
      break;
    }
    case REDIS_REPLY_INTEGER: {
      *out = common::Value::CreateLongLongIntegerValue(r->integer);
      break;
    }
    case REDIS_REPLY_ARRAY: {
      common::ArrayValue* arv = common::Value::CreateArrayValue();
      for (size_t i = 0; i < r->elements; ++i) {
        common::Value* val = nullptr;
        common::Error err = ValueFromReplay(r->element[i], &val);
        if (err) {
          delete arv;
          return err;
        }
        arv->Append(val);
      }
      *out = arv;
      break;
    }
    default: { return common::make_error(common::MemSPrintf("Unknown reply type: %d", r->type)); }
  }

  return common::Error();
}

common::Error ExecRedisCommand(NativeConnection* c,
                               size_t argc,
                               const char** argv,
                               const size_t* argvlen,
                               redisReply** out_reply) {
  if (!c) {
    DNOTREACHED();
    return common::make_error("Not connected");
  }

  if (argc == 0 || !argv || !out_reply) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  int res = redisAppendCommandArgv(c, argc, argv, argvlen);
  if (res == REDIS_ERR) {
    DNOTREACHED();
    return PrintRedisContextError(c);
  }

  void* reply = nullptr;
  res = redisGetReply(c, &reply);
  if (res == REDIS_ERR) {
    /* Filter cases where we should reconnect */
    if (c->err == REDIS_ERR_IO && errno == ECONNRESET) {
      return common::make_error("Needed reconnect.");
    }
    if (c->err == REDIS_ERR_EOF) {
      return common::make_error("Needed reconnect.");
    }

    return PrintRedisContextError(c);
  }

  redisReply* rreply = static_cast<redisReply*>(reply);
  if (rreply->type == REDIS_REPLY_ERROR) {
    if (!strncmp(rreply->str, "MOVED", 5) || !strcmp(rreply->str, "ASK")) {
      char* p = rreply->str;
      char* s = strchr(p, ' '); /* MOVED[S]3999 127.0.0.1:6381 */
      p = strchr(s + 1, ' ');   /* MOVED[S]3999[P]127.0.0.1:6381 */
      *p = '\0';
      int slot = atoi(s + 1);
      s = strrchr(p + 1, ':'); /* MOVED 3999[P]127.0.0.1[S]6381 */
      *s = '\0';

      std::string hostip = (p + 1);
      int port = atoi(s + 1);
      freeReplyObject(rreply);
      common::Error err = common::make_error(common::COMMON_EINTR);
      err->SetPayload(new common::net::HostAndPortAndSlot(hostip, port, slot));
      return err;
    }
    std::string str(rreply->str, rreply->len);
    freeReplyObject(rreply);
    return common::make_error(str);
  }

  *out_reply = rreply;
  return common::Error();
}

common::Error ExecRedisCommand(NativeConnection* c, const commands_args_t& argv, redisReply** out_reply) {
  if (argv.empty() || !out_reply) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  const char** argvc = static_cast<const char**>(calloc(sizeof(const char*), argv.size()));
  int argcc = 0;
  size_t* argvlen = reinterpret_cast<size_t*>(malloc(argv.size() * sizeof(size_t)));
  for (size_t i = 0; i < argv.size(); ++i) {
    argvc[i] = reinterpret_cast<const char*>(argv[i].data());  // safe
    argvlen[i] = argv[i].size();
    argcc++;
  }

  common::Error err = ExecRedisCommand(c, argcc, argvc, argvlen, out_reply);
  free(argvlen);
  free(argvc);
  return err;
}

common::Error ExecRedisCommand(NativeConnection* c, const command_buffer_t& command, redisReply** out_reply) {
  if (command.empty() || !out_reply) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  commands_args_t standart_argv;
  if (!ParseCommandLine(command, &standart_argv)) {
    return common::make_error_inval();
  }

  size_t argc = standart_argv.size();
  void* raw_argvlen_ptr = malloc(argc * sizeof(size_t));
  void* raw_argv_ptr = malloc(argc * sizeof(char*));

  size_t* argvlen = reinterpret_cast<size_t*>(raw_argvlen_ptr);
  const char** argv = reinterpret_cast<const char**>(raw_argv_ptr);
  for (size_t i = 0; i < argc; ++i) {
    argv[i] = standart_argv[i].data();  // safe
    argvlen[i] = standart_argv[i].size();
  }

  common::Error err = ExecRedisCommand(c, argc, argv, argvlen, out_reply);
  free(raw_argv_ptr);
  free(raw_argvlen_ptr);
  return err;
}

common::Error AuthContext(NativeConnection* context, const command_buffer_t& password) {
  if (password.empty()) {
    return common::Error();
  }

  redisReply* reply = nullptr;
  const commands_args_t cmds = {GEN_CMD_STRING("AUTH"), password};
  common::Error err = ExecRedisCommand(context, cmds, &reply);
  if (err) {
    return err;
  }
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Connect(const config_t& config) {
  common::Error err = base_class::Connect(config);
  if (err) {
    return err;
  }

  /* Do AUTH and select the right DB. */
  err = Auth(common::ConvertToCharBytes(config->auth));  // convert from std::string to char bytes
  if (err) {
    return err;
  }

  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Disconnect() {
  cur_db_ = invalid_db_num;
  is_auth_ = false;
  return base_class::Disconnect();
}

template <typename Config, ConnectionType ContType>
db_name_t DBConnection<Config, ContType>::GetCurrentDBName() const {
  if (IsAuthenticated()) {
    auto config = base_class::GetConfig();
    int db_num = config->db_num;
    return cur_db_ != invalid_db_num ? common::ConvertToCharBytes(cur_db_)
                                     : common::ConvertToCharBytes(db_num);  // convert from std::string to db_name_t
  }

  DNOTREACHED();
  return base_class::GetCurrentDBName();
}

template <typename Config, ConnectionType ContType>
bool DBConnection<Config, ContType>::IsAuthenticated() const {
  if (!base_class::IsAuthenticated()) {
    return false;
  }

  return is_auth_;
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::CommonExec(const commands_args_t& argv, FastoObject* out) {
  if (!out || argv.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, argv, &reply);
  if (err) {
    return err;
  }

  err = CliFormatReplyRaw(out, reply);
  freeReplyObject(reply);
  return err;
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::CliFormatReplyRaw(FastoObject* out, redisReply* r) {
  if (!out || !r) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Value* out_val = nullptr;
  common::Error err = ValueFromReplay(r, &out_val);
  if (err) {
    if (err->GetDescription() == "NOAUTH") {  // "NOAUTH Authentication
                                              // required."
      is_auth_ = false;
    }
    return err;
  }

  FastoObject* obj = new FastoObject(out, out_val, base_class::GetDelimiter());
  out->AddChildren(obj);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::CliReadReply(FastoObject* out) {
  if (!out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsConnected();
  if (err) {
    return err;
  }

  void* _reply = nullptr;
  if (redisGetReply(base_class::connection_.handle_, &_reply) != REDIS_OK) {
    /* Filter cases where we should reconnect */
    if (base_class::connection_.handle_->err == REDIS_ERR_IO && errno == ECONNRESET) {
      return common::make_error("Needed reconnect.");
    }
    if (base_class::connection_.handle_->err == REDIS_ERR_EOF) {
      return common::make_error("Needed reconnect.");
    }

    return PrintRedisContextError(base_class::connection_.handle_); /* avoid compiler warning */
  }

  redisReply* reply = static_cast<redisReply*>(_reply);
  common::Error er = CliFormatReplyRaw(out, reply);
  freeReplyObject(reply);
  return er;
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Auth(const command_buffer_t& password) {
  common::Error err = base_class::TestIsConnected();
  if (err) {
    return err;
  }

  err = AuthContext(base_class::connection_.handle_, password);
  if (err) {
    is_auth_ = false;
    return err;
  }

  is_auth_ = true;
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SetClientName(const std::string& name) {
  if (name.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  command_buffer_writer_t wr;
  wr << "CLIENT SETNAME " << name;

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, wr.str(), &reply);
  if (err) {
    return err;
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Lpush(const NKey& key, NValue arr, redis_int_t* list_len) {
  if (!arr || arr->GetType() != common::Value::TYPE_ARRAY || !list_len) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << "LPUSH " << key_str.GetForCommandLine() << SPACE_STR << arr.GetForCommandLine();

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, wr.str(), &reply);
  if (err) {
    return err;
  }

  NDbKValue rarr;
  rarr.SetKey(key);

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    common::ArrayValue* carr = static_cast<common::ArrayValue*>(arr.get());
    if (carr->GetSize() == static_cast<size_t>(reply->integer)) {
      rarr.SetValue(arr);
    } else {
      rarr.SetValue(NValue(common::Value::CreateArrayValue()));
    }
    base_class::client_->OnAddedKey(rarr);
  }
  *list_len = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Rpush(const NKey& key, NValue arr, redis_int_t* list_len) {
  if (!arr || arr->GetType() != common::Value::TYPE_ARRAY || !list_len) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  const auto key_str = key.GetKey();

  command_buffer_writer_t wr;
  wr << "RPUSH " << key_str.GetForCommandLine() << SPACE_STR << arr.GetForCommandLine();

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, wr.str(), &reply);
  if (err) {
    return err;
  }

  NDbKValue rarr;
  rarr.SetKey(key);

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    common::ArrayValue* carr = static_cast<common::ArrayValue*>(arr.get());
    if (carr->GetSize() == static_cast<size_t>(reply->integer)) {
      rarr.SetValue(arr);
    } else {
      rarr.SetValue(NValue(common::Value::CreateArrayValue()));
    }
    base_class::client_->OnAddedKey(rarr);
  }
  *list_len = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::LfastoSet(const NKey& key, NValue arr, redis_int_t* list_len) {
  if (!arr || arr->GetType() != common::Value::TYPE_ARRAY || !list_len) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  ttl_t ttl;
  err = base_class::GetTTL(key, &ttl);
  if (err) {
    return err;
  }

  NKeys keys;
  err = base_class::Delete({key}, &keys);
  if (err) {
    return err;
  }

  err = Lpush(key, arr, list_len);
  if (err) {
    return err;
  }

  if (ttl == NO_TTL || ttl == EXPIRED_TTL) {
    return common::Error();
  }

  return base_class::SetTTL(key, ttl);
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Lrange(const NKey& key, int start, int stop, NDbKValue* loaded_key) {
  if (!loaded_key) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = LrangeImpl(key, start, stop, loaded_key);
  if (err) {
    return err;
  }

  if (base_class::client_) {
    base_class::client_->OnLoadedKey(*loaded_key);
  }

  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Mget(const std::vector<NKey>& keys, std::vector<NDbKValue>* loaded_keys) {
  if (keys.empty() || !loaded_keys) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t mget_cmd;
  err = tran->Mget(keys, &mget_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, mget_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  for (size_t i = 0; i < reply->elements; ++i) {
    redisReply* key_value = reply->element[i];
    common::Value* val = nullptr;
    common::Error err = ValueFromReplay(key_value, &val);
    if (err) {
      delete val;
      freeReplyObject(reply);
      return err;
    }

    NDbKValue loaded_key(keys[i], NValue(val));
    if (base_class::client_) {
      base_class::client_->OnLoadedKey(loaded_key);
    }
    loaded_keys->push_back(loaded_key);
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Mset(const std::vector<NDbKValue>& keys) {
  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t mset_cmd;
  err = tran->Mset(keys, &mset_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, mset_cmd, &reply);
  if (err) {
    return err;
  }

  if (base_class::client_) {
    for (size_t i = 0; i < keys.size(); ++i) {
      base_class::client_->OnAddedKey(keys[i]);
    }
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::MsetNX(const std::vector<NDbKValue>& keys, redis_int_t* result) {
  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t msetnx_cmd;
  err = tran->MsetNX(keys, &msetnx_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, msetnx_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_ && reply->integer) {
    for (size_t i = 0; i < keys.size(); ++i) {
      base_class::client_->OnAddedKey(keys[i]);
    }
  }

  *result = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Append(const NKey& key, const NValue& val, redis_int_t* new_string_len) {
  if (!new_string_len) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t append_cmd;
  err = tran->Append(key, val, &append_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, append_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    const readable_string_t data = val.GetData();
    const bool is_created = static_cast<size_t>(reply->integer) == data.size();
    if (is_created) {
      base_class::client_->OnAddedKey(NDbKValue(key, val));
    }
  }

  *new_string_len = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBConnection::SetEx(const NDbKValue& key, ttl_t ttl) {
  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t setex_cmd;
  err = tran->SetEx(key, ttl, &setex_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, setex_cmd, &reply);
  if (err) {
    return err;
  }

  DCHECK(key.GetType() == common::Value::TYPE_STRING);
  if (base_class::client_) {
    base_class::client_->OnAddedKey(key);
  }
  if (base_class::client_) {
    base_class::client_->OnChangedKeyTTL(key.GetKey(), ttl);
  }
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBConnection::SetNX(const NDbKValue& key, redis_int_t* result) {
  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t setnx_cmd;
  err = tran->SetNX(key, &setnx_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, setnx_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_ && reply->integer) {
    base_class::client_->OnAddedKey(key);
  }

  *result = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Decr(const NKey& key, redis_int_t* decr) {
  if (!decr) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  command_buffer_t decr_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  err = tran->Decr(key, &decr_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, decr_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    auto result_str = common::ConvertToCharBytes(reply->integer);
    NValue val(common::Value::CreateStringValue(result_str));  // type of key should be string
    base_class::client_->OnAddedKey(NDbKValue(key, val));
  }
  *decr = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBConnection::DecrBy(const NKey& key, int dec, redis_int_t* decr) {
  if (!decr) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  command_buffer_t decrby_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  err = tran->DecrBy(key, dec, &decrby_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, decrby_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    auto result_str = common::ConvertToCharBytes(reply->integer);
    NValue val(common::Value::CreateStringValue(result_str));  // type of key should be string
    base_class::client_->OnAddedKey(NDbKValue(key, val));
  }
  *decr = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBConnection::Incr(const NKey& key, redis_int_t* incr) {
  if (!incr) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  command_buffer_t incr_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  err = tran->Incr(key, &incr_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, incr_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    auto result_str = common::ConvertToCharBytes(reply->integer);
    NValue val(common::Value::CreateStringValue(result_str));  // type of key should be string
    base_class::client_->OnAddedKey(NDbKValue(key, val));
  }
  *incr = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBConnection::IncrBy(const NKey& key, int inc, redis_int_t* incr) {
  if (!incr) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  command_buffer_t incrby_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  err = tran->IncrBy(key, inc, &incrby_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, incrby_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    auto result_str = common::ConvertToCharBytes(reply->integer);
    NValue val(common::Value::CreateStringValue(result_str));  // type of key should be string
    base_class::client_->OnAddedKey(NDbKValue(key, val));
  }
  *incr = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::IncrByFloat(const NKey& key, double inc, command_buffer_t* str_incr) {
  if (!str_incr) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  command_buffer_t incrfloat_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  err = tran->IncrByFloat(key, inc, &incrfloat_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, incrfloat_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_STRING) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  command_buffer_t str = GEN_CMD_STRING_SIZE(reply->str, reply->len);
  if (base_class::client_) {
    NValue val(common::Value::CreateStringValue(str));
    base_class::client_->OnAddedKey(NDbKValue(key, val));
  }
  *str_incr = str;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::PExpire(const NKey& key, ttl_t ttl) {
  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t ttl_cmd;
  err = tran->PExpire(key, ttl, &ttl_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, ttl_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->integer == 0) {
    return base_class::GenerateError(REDIS_CHANGE_PTTL_COMMAND, "key does not exist or the timeout could not be set.");
  }

  if (base_class::client_) {
    base_class::client_->OnChangedKeyTTL(key, ttl);
  }
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::PTTL(const NKey& key, pttl_t* ttl) {
  if (!ttl) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t pttl_cmd;
  err = tran->PTTL(key, &pttl_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, pttl_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    freeReplyObject(reply);
    return base_class::GenerateError(REDIS_GET_PTTL_COMMAND, "command internal error");
  }

  *ttl = reply->integer;
  ttl_t ttl_sec = *ttl;
  if (ttl_sec == NO_TTL || ttl_sec == EXPIRED_TTL) {
  } else {
    ttl_sec = ttl_sec / 1000;
  }
  freeReplyObject(reply);

  if (base_class::client_) {
    base_class::client_->OnLoadedKeyTTL(key, ttl_sec);
  }

  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Sadd(const NKey& key, NValue set, redis_int_t* added) {
  if (!set || set->GetType() != common::Value::TYPE_SET || !added) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  NDbKValue rset(key, set);
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t sadd_cmd;
  err = tran->CreateKeyCommand(rset, &sadd_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, sadd_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    base_class::client_->OnAddedKey(rset);
  }
  *added = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Smembers(const NKey& key, NDbKValue* loaded_key) {
  if (!loaded_key) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = SmembersImpl(key, loaded_key);
  if (err) {
    return err;
  }

  if (base_class::client_) {
    base_class::client_->OnLoadedKey(*loaded_key);
  }
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Zadd(const NKey& key, NValue scores, redis_int_t* added) {
  if (!scores || scores->GetType() != common::Value::TYPE_ZSET || !added) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  NDbKValue rzset(key, scores);
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t zadd_cmd;
  err = tran->CreateKeyCommand(rzset, &zadd_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, zadd_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    base_class::client_->OnAddedKey(rzset);
  }
  *added = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Zrange(const NKey& key,
                                                     int start,
                                                     int stop,
                                                     bool withscores,
                                                     NDbKValue* loaded_key) {
  if (!loaded_key) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = ZrangeImpl(key, start, stop, withscores, loaded_key);
  if (err) {
    return err;
  }

  if (base_class::client_) {
    base_class::client_->OnLoadedKey(*loaded_key);
  }
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Hmset(const NKey& key, NValue hash) {
  if (!hash || hash->GetType() != common::Value::TYPE_HASH) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  NDbKValue rhash(key, hash);
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t hmset_cmd;
  err = tran->CreateKeyCommand(rhash, &hmset_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, hmset_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_STATUS) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (base_class::client_) {
    base_class::client_->OnAddedKey(rhash);
  }
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Hgetall(const NKey& key, NDbKValue* loaded_key) {
  if (!loaded_key) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  err = HgetallImpl(key, loaded_key);
  if (err) {
    return err;
  }

  if (base_class::client_) {
    base_class::client_->OnLoadedKey(*loaded_key);
  }
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Monitor(const commands_args_t& argv, FastoObject* out) {
  if (!out || argv.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, argv, &reply);
  if (err) {
    return err;
  }

  err = CliFormatReplyRaw(out, reply);
  freeReplyObject(reply);
  reply = nullptr;
  if (err) {
    return err;
  }

  while (!base_class::IsInterrupted()) {  // listen loop
    err = CliReadReply(out);
    if (err) {
      return err;
    }
  }

  return common::make_error(common::COMMON_EINTR);
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::Subscribe(const commands_args_t& argv, FastoObject* out) {
  if (!out || argv.empty()) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, argv, &reply);
  if (err) {
    return err;
  }

  err = CliFormatReplyRaw(out, reply);
  freeReplyObject(reply);
  reply = nullptr;
  if (err) {
    return err;
  }

  while (!base_class::IsInterrupted()) {  // listen loop
    err = CliReadReply(out);
    if (err) {
      return err;
    }
  }

  return common::make_error(common::COMMON_EINTR);
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SlaveMode(FastoObject* out) {
  if (!out) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  unsigned long long payload = 0;
  err = SendSync(&payload);
  if (err) {
    return err;
  }

  char buf[1024];
  /* Discard the payload. */
  while (payload) {
    ssize_t nread = 0;
    int size = (payload > sizeof(buf)) ? sizeof(buf) : payload;
    int res = redisReadToBuffer(base_class::connection_.handle_, buf, size, &nread);
    if (res == REDIS_ERR) {
      return common::make_error("Error reading RDB payload while SYNCing");
    }
    payload -= nread;
  }

  /* Now we can use hiredis to read the incoming protocol.
   */
  while (!base_class::IsInterrupted()) {  // listen loop
    err = CliReadReply(out);
    if (err) {
      return err;
    }
  }

  return common::make_error(common::COMMON_EINTR);
}

/* Sends SYNC and reads the number of bytes in the payload.
 * Used both by
 * slaveMode() and getRDB(). */
template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SendSync(unsigned long long* payload) {
  if (!payload) {
    DNOTREACHED();
    return common::make_error_inval();
  }
  /* To start we need to send the SYNC command and return
   * the payload.
   * The hiredis client lib does not understand this part of
   * the protocol
   * and we don't want to mess with its buffers, so
   * everything is performed
   * using direct low-level I/O. */
  char buf[4096], *p;

  /* Send the SYNC command. */
  ssize_t nwrite = 0;
  if (redisWriteFromBuffer(base_class::connection_.handle_, "SYNC\r\n", &nwrite) == REDIS_ERR) {
    return common::make_error("Error writing to master");
  }

  /* Read $<payload>\r\n, making sure to read just up to
   * "\n" */
  p = buf;
  while (1) {
    ssize_t nread = 0;
    int res = redisReadToBuffer(base_class::connection_.handle_, p, 1, &nread);
    if (res == REDIS_ERR) {
      return common::make_error("Error reading bulk length while SYNCing");
    }

    if (!nread) {
      continue;
    }

    if (*p == END_LINE_CHAR && p != buf) {
      break;
    }
    if (*p != END_LINE_CHAR) {
      p++;
    }
  }
  *p = '\0';
  if (buf[0] == '-') {
    std::string buf2 = common::MemSPrintf("SYNC with master failed: %s", buf);
    return common::make_error(buf2);
  }

  *payload = strtoull(buf + 1, nullptr, 10);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::LrangeImpl(const NKey& key, int start, int stop, NDbKValue* loaded_key) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t lrange_cmd;
  common::Error err = tran->Lrange(key, start, stop, &lrange_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, lrange_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  common::Value* val = nullptr;
  err = ValueFromReplay(reply, &val);
  if (err) {
    delete val;
    freeReplyObject(reply);
    return err;
  }

  *loaded_key = NDbKValue(key, NValue(val));
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SmembersImpl(const NKey& key, NDbKValue* loaded_key) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t smembers_cmd;
  common::Error err = tran->Smembers(key, &smembers_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, smembers_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  common::Value* val = nullptr;
  err = ValueFromReplay(reply, &val);
  if (err) {
    delete val;
    freeReplyObject(reply);
    return err;
  }

  common::ArrayValue* arr = nullptr;
  if (!val->GetAsList(&arr)) {
    delete val;
    freeReplyObject(reply);
    return common::make_error("Conversion error array to set");
  }

  common::SetValue* set = common::Value::CreateSetValue();
  for (size_t i = 0; i < arr->GetSize(); ++i) {
    common::Value* lval = nullptr;
    if (arr->Get(i, &lval)) {
      set->Insert(lval->DeepCopy());
    }
  }

  delete val;
  *loaded_key = NDbKValue(key, NValue(set));
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::HgetallImpl(const NKey& key, NDbKValue* loaded_key) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t hgetall_cmd;
  common::Error err = tran->Hgetall(key, &hgetall_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, hgetall_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  common::Value* val = nullptr;
  err = ValueFromReplay(reply, &val);
  if (err) {
    delete val;
    freeReplyObject(reply);
    return err;
  }

  common::ArrayValue* arr = nullptr;
  if (!val->GetAsList(&arr)) {
    delete val;
    freeReplyObject(reply);
    return common::make_error("Conversion error array to hash");
  }

  common::HashValue* hash = common::Value::CreateHashValue();
  for (size_t i = 0; i < arr->GetSize(); i += 2) {
    common::Value* lkey = nullptr;
    common::Value* lvalue = nullptr;
    if (arr->Get(i, &lkey) && arr->Get(i + 1, &lvalue)) {
      hash->Insert(lkey->DeepCopy(), lvalue->DeepCopy());
    }
  }

  delete val;
  *loaded_key = NDbKValue(key, NValue(hash));
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::ZrangeImpl(const NKey& key,
                                                         int start,
                                                         int stop,
                                                         bool withscores,
                                                         NDbKValue* loaded_key) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t zrange;
  common::Error err = tran->Zrange(key, start, stop, withscores, &zrange);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, zrange, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  common::Value* val = nullptr;
  err = ValueFromReplay(reply, &val);
  if (err) {
    delete val;
    freeReplyObject(reply);
    return err;
  }

  if (!withscores) {
    *loaded_key = NDbKValue(key, NValue(val));
    if (base_class::client_) {
      base_class::client_->OnLoadedKey(*loaded_key);
    }
    freeReplyObject(reply);
    return common::Error();
  }

  common::ArrayValue* arr = nullptr;
  if (!val->GetAsList(&arr)) {
    delete val;
    freeReplyObject(reply);
    return common::make_error("Conversion error array to zset");
  }

  common::ZSetValue* zset = common::Value::CreateZSetValue();
  for (size_t i = 0; i < arr->GetSize(); i += 2) {
    common::Value* lmember = nullptr;
    common::Value* lscore = nullptr;
    if (arr->Get(i, &lmember) && arr->Get(i + 1, &lscore)) {
      zset->Insert(lscore->DeepCopy(), lmember->DeepCopy());
    }
  }

  delete val;
  *loaded_key = NDbKValue(key, NValue(zset));
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::ScanImpl(cursor_t cursor_in,
                                                       const pattern_t& pattern,
                                                       keys_limit_t count_keys,
                                                       raw_keys_t* keys_out,
                                                       cursor_t* cursor_out) {
  const command_buffer_t pattern_result = GetKeysPattern(cursor_in, pattern, count_keys);
  redisReply* reply = nullptr;
  common::Error err = ExecRedisCommand(base_class::connection_.handle_, pattern_result, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  common::Value* val = nullptr;
  err = ValueFromReplay(reply, &val);
  freeReplyObject(reply);
  reply = nullptr;
  if (err) {
    delete val;
    return err;
  }

  common::ArrayValue* arr = static_cast<common::ArrayValue*>(val);
  CHECK_EQ(2, arr->GetSize());
  command_buffer_t cursor_out_str;
  if (!arr->GetString(0, &cursor_out_str)) {
    delete val;
    return common::make_error("I/O error");
  }

  common::ArrayValue* arr_keys = nullptr;
  if (!arr->GetList(1, &arr_keys)) {
    delete val;
    return common::make_error("I/O error");
  }

  for (size_t i = 0; i < arr_keys->GetSize(); ++i) {
    command_buffer_t key;
    if (arr_keys->GetString(i, &key)) {
      keys_out->push_back(key);
    }
  }

  cursor_t lcursor_out;
  if (!common::ConvertFromBytes(cursor_out_str, &lcursor_out)) {
    return common::make_error_inval();
  }

  *cursor_out = lcursor_out;
  delete val;
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::KeysImpl(const raw_key_t& key_start,
                                                       const raw_key_t& key_end,
                                                       keys_limit_t limit,
                                                       raw_keys_t* ret) {
  UNUSED(key_start);
  UNUSED(key_end);
  UNUSED(limit);
  UNUSED(ret);
  return ICommandTranslator::NotSupported(GEN_CMD_STRING(DB_KEYS_COMMAND));
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBKeysCountImpl(keys_limit_t* size) {
  redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(base_class::connection_.handle_, DBSIZE));

  if (!reply || reply->type != REDIS_REPLY_INTEGER) {
    return common::make_error("Couldn't determine " DB_DBKCOUNT_COMMAND "!");
  }

  /* Grab the number of keys and free our reply */
  *size = static_cast<keys_limit_t>(reply->integer);
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::FlushDBImpl() {
  redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(base_class::connection_.handle_, DB_FLUSHDB_COMMAND));
  if (!reply) {
    return PrintRedisContextError(base_class::connection_.handle_);
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SelectImpl(const db_name_t& name, IDataBaseInfo** info) {
  int num;
  if (!common::ConvertFromBytes(name, &num)) {
    return common::make_error_inval();
  }

  command_buffer_t select_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  common::Error err = tran->SelectDBCommand(name, &select_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, select_cmd, &reply);
  if (err) {
    return err;
  }

  base_class::connection_.config_->db_num = num;
  cur_db_ = num;
  keys_limit_t sz = 0;
  err = base_class::DBKeysCount(&sz);
  DCHECK(!err);
  DataBaseInfo* linfo = new DataBaseInfo(name, true, sz);
  *info = linfo;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DeleteImpl(const NKeys& keys, NKeys* deleted_keys) {
  for (size_t i = 0; i < keys.size(); ++i) {
    NKey key = keys[i];
    command_buffer_t del_cmd;
    redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
    common::Error err = tran->DeleteKeyCommand(key, &del_cmd);
    if (err) {
      return err;
    }

    redisReply* reply = nullptr;
    err = ExecRedisCommand(base_class::connection_.handle_, del_cmd, &reply);
    if (err) {
      return err;
    }

    if (reply->type != REDIS_REPLY_INTEGER) {
      DNOTREACHED() << "Unexpected type: " << reply->type;
      freeReplyObject(reply);
      return common::make_error("I/O error");
    }

    if (reply->integer == 1) {
      deleted_keys->push_back(key);
    }
    freeReplyObject(reply);
  }

  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SetImpl(const NDbKValue& key) {
  command_buffer_t set_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  common::Error err = tran->CreateKeyCommand(key, &set_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, set_cmd, &reply);
  if (err) {
    return err;
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::GetImpl(const NKey& key, NDbKValue* loaded_key) {
  command_buffer_t get_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  common::Error err = tran->LoadKeyCommand(key, common::Value::TYPE_STRING, &get_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, get_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type == REDIS_REPLY_NIL) {
    freeReplyObject(reply);
    return base_class::GenerateError(DB_GET_KEY_COMMAND, "key not found.");
  }

  if (reply->type != REDIS_REPLY_STRING) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  common::Value* val = common::Value::CreateStringValue(GEN_CMD_STRING_SIZE(reply->str, reply->len));
  *loaded_key = NDbKValue(key, NValue(val));
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::GetTypeImpl(const NKey& key, readable_string_t* type) {
  command_buffer_t get_type_cmd;
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  common::Error err = tran->GetTypeCommand(key, &get_type_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, get_type_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_STATUS) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  const auto type_str = GEN_CMD_STRING_SIZE(reply->str, reply->len);
  if (type_str == GEN_CMD_STRING("none")) {
    freeReplyObject(reply);
    return base_class::GenerateError(DB_KEY_TYPE_COMMAND, "key not found.");
  }

  *type = type_str;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::GetUniImpl(const NKey& key, NDbKValue* loaded_key) {
  readable_string_t type_str;
  common::Error err = base_class::GetType(key, &type_str);
  if (err) {
    return err;
  }

  if (type_str == GEN_CMD_STRING("string")) {
    return GetImpl(key, loaded_key);
  } else if (type_str == GEN_CMD_STRING("list")) {
    return LrangeImpl(key, 0, -1, loaded_key);
  } else if (type_str == GEN_CMD_STRING("set")) {
    return SmembersImpl(key, loaded_key);
  } else if (type_str == GEN_CMD_STRING("hash")) {
    return HgetallImpl(key, loaded_key);
  } else if (type_str == GEN_CMD_STRING("zset")) {
    return ZrangeImpl(key, 0, -1, true, loaded_key);
  }

  std::stringstream wr;
  wr << "Unknown type: " << common::ConvertToString(type_str);
  return common::make_error(wr.str());
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::RenameImpl(const NKey& key, const nkey_t& new_key) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t rename_cmd;
  common::Error err = tran->RenameKeyCommand(key, new_key, &rename_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, rename_cmd, &reply);
  if (err) {
    return err;
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::SetTTLImpl(const NKey& key, ttl_t ttl) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t ttl_cmd;
  // PERSIST/EXPIRE
  common::Error err = tran->ChangeKeyTTLCommand(key, ttl, &ttl_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, ttl_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  if (reply->integer == 0) {  // key not exists
    freeReplyObject(reply);
    return base_class::GenerateError(DB_SET_TTL_COMMAND, "key does not exist or the timeout could not be set.");
  }

  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::GetTTLImpl(const NKey& key, ttl_t* ttl) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t ttl_cmd;
  common::Error err = tran->LoadKeyTTLCommand(key, &ttl_cmd);
  if (err) {
    return err;
  }

  redisReply* reply = nullptr;
  err = ExecRedisCommand(base_class::connection_.handle_, ttl_cmd, &reply);
  if (err) {
    return err;
  }

  if (reply->type != REDIS_REPLY_INTEGER) {
    DNOTREACHED() << "Unexpected type: " << reply->type;
    freeReplyObject(reply);
    return common::make_error("I/O error");
  }

  *ttl = reply->integer;
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::QuitImpl() {
  redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(base_class::connection_.handle_, DB_QUIT_COMMAND));
  if (!reply) {
    return PrintRedisContextError(base_class::connection_.handle_);
  }

  freeReplyObject(reply);
  common::Error err = Disconnect();
  UNUSED(err);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::ConfigGetDatabasesImpl(db_names_t* dbs) {
  redis_translator_t tran = base_class::template GetSpecificTranslator<CommandTranslator>();
  command_buffer_t get_dbs_cmd;
  common::Error err = tran->GetDatabasesCommand(&get_dbs_cmd);
  if (err) {
    return err;
  }

  redisReply* reply =
      reinterpret_cast<redisReply*>(redisCommand(base_class::connection_.handle_, DB_GET_DATABASES_COMMAND));
  if (!reply) {
    return PrintRedisContextError(base_class::connection_.handle_);
  }

  if (reply->type == REDIS_REPLY_ARRAY && reply->elements == 2) {
    size_t count_dbs = 0;
    const std::string count_dbs_str(reply->element[1]->str, reply->element[1]->len);
    if (common::ConvertFromString(count_dbs_str, &count_dbs)) {
      for (size_t i = 0; i < count_dbs; ++i) {
        dbs->push_back(common::ConvertToCharBytes(i));  // convert from std::string to char bytes
      }
      freeReplyObject(reply);
      return common::Error();
    }
  }

  *dbs = {GetCurrentDBName()};
  freeReplyObject(reply);
  return common::Error();
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::DBSize(keys_limit_t* size) {
  if (!size) {
    DNOTREACHED();
    return common::make_error_inval();
  }

  return base_class::DBKeysCount(size);
}

template <typename Config, ConnectionType ContType>
common::Error DBConnection<Config, ContType>::ExecuteAsPipeline(
    const std::vector<FastoObjectCommandIPtr>& cmds,
    void (*log_command_cb)(FastoObjectCommandIPtr command)) {
  if (cmds.empty()) {
    return common::make_error("Invalid input command");
  }

  common::Error err = base_class::TestIsAuthenticated();
  if (err) {
    return err;
  }

  // start piplene mode
  std::vector<FastoObjectCommandIPtr> valid_cmds;
  for (size_t i = 0; i < cmds.size(); ++i) {
    FastoObjectCommandIPtr cmd = cmds[i];
    command_buffer_t command = cmd->GetInputCommand();
    if (command.empty()) {
      continue;
    }

    if (log_command_cb) {
      log_command_cb(cmd);
    }

    commands_args_t standart_argv;
    if (!ParseCommandLine(command, &standart_argv)) {
      return common::make_error_inval();
    }

    if (IsPipeLineCommand(standart_argv[0])) {
      valid_cmds.push_back(cmd);

      size_t argc = standart_argv.size();
      void* raw_argvlen_ptr = malloc(argc * sizeof(size_t));
      void* raw_argv_ptr = malloc(argc * sizeof(char*));

      size_t* argvlen = reinterpret_cast<size_t*>(raw_argvlen_ptr);
      const char** argv = reinterpret_cast<const char**>(raw_argv_ptr);
      for (size_t i = 0; i < argc; ++i) {
        argv[i] = standart_argv[i].data();  // safe
        argvlen[i] = standart_argv[i].size();
      }

      redisAppendCommandArgv(base_class::connection_.handle_, argc, argv, argvlen);
      free(raw_argv_ptr);
      free(raw_argvlen_ptr);
    }
  }

  for (size_t i = 0; i < valid_cmds.size(); ++i) {
    FastoObjectCommandIPtr cmd = cmds[i];
    common::Error err = CliReadReply(cmd.get());
    if (err) {
      return err;
    }
  }
  // end piplene

  return common::Error();
}
}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql

#include <fastonosql/core/db/pika/config.h>
#include <fastonosql/core/db/redis/config.h>

namespace fastonosql {
namespace core {
namespace redis_compatible {
#if defined(BUILD_WITH_REDIS)
template class DBConnection<redis::RConfig, REDIS>;
#endif
#if defined(BUILD_WITH_PIKA)
template class DBConnection<pika::RConfig, PIKA>;
#endif
}  // namespace redis_compatible
}  // namespace core
}  // namespace fastonosql
