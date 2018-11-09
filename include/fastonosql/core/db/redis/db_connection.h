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

#include <vector>

#include <fastonosql/core/db/redis/command_translator.h>
#include <fastonosql/core/db/redis_compatible/db_connection.h>

#include <fastonosql/core/db/redis/config.h>

namespace fastonosql {
namespace core {
#if defined(PRO_VERSION)
class IModuleConnectionClient;
#endif
namespace redis {

typedef redis_compatible::NativeConnection NativeConnection;

common::Error CreateConnection(const RConfig& config, NativeConnection** context);
common::Error TestConnection(const RConfig& config);
#if defined(PRO_VERSION)
common::Error DiscoveryClusterConnection(const RConfig& config, std::vector<ServerDiscoveryClusterInfoSPtr>* infos);
common::Error DiscoverySentinelConnection(const RConfig& config, std::vector<ServerDiscoverySentinelInfoSPtr>* infos);
#endif

class DBConnection : public redis_compatible::DBConnection<RConfig, REDIS> {
 public:
  typedef std::shared_ptr<CommandTranslator> redis_translator_t;
  typedef redis_compatible::DBConnection<RConfig, REDIS> base_class;
#if defined(PRO_VERSION)
  explicit DBConnection(CDBConnectionClient* client, IModuleConnectionClient* mclient);
#else
  explicit DBConnection(CDBConnectionClient* client);
#endif

  common::Error GraphQuery(const commands_args_t& argv, FastoObject* out) WARN_UNUSED_RESULT;
  common::Error GraphExplain(const commands_args_t& argv, FastoObject* out) WARN_UNUSED_RESULT;
  common::Error GraphDelete(const commands_args_t& argv, FastoObject* out) WARN_UNUSED_RESULT;

  common::Error JsonSet(const NDbKValue& key) WARN_UNUSED_RESULT;
  common::Error JsonGet(const NKey& key, NDbKValue* loaded_key) WARN_UNUSED_RESULT;
  common::Error JsonDel(const NKey& key, long long* deleted) WARN_UNUSED_RESULT;

  // stream
  common::Error XAdd(const NDbKValue& key, readable_string_t* gen_id) WARN_UNUSED_RESULT;
  common::Error XRange(const NKey& key, NDbKValue* loaded_key, FastoObject* out) WARN_UNUSED_RESULT;
  common::Error XfastoSet(const NKey& key, NValue stream) WARN_UNUSED_RESULT;

  bool IsInternalCommand(const command_buffer_t& command_name);
#if defined(PRO_VERSION)
  common::Error ModuleLoad(const ModuleInfo& module) WARN_UNUSED_RESULT;    // nvi
  common::Error ModuleUnLoad(const ModuleInfo& module) WARN_UNUSED_RESULT;  // nvi
#endif

 private:
  common::Error JsonSetImpl(const NDbKValue& key);
  common::Error JsonGetImpl(const NKey& key, NDbKValue* loaded_key);
  common::Error JsonDelImpl(const NKey& key, long long* deleted);

  common::Error XAddImpl(const NDbKValue& key, command_buffer_t* gen_id);
  common::Error XRangeImpl(const NKey& key, NDbKValue* loaded_key, FastoObject* out);
  common::Error XRangeImpl2(const NKey& key, NDbKValue* loaded_key);

  common::Error GetUniImpl(const NKey& key, NDbKValue* loaded_key) override;

#if defined(PRO_VERSION)
  virtual common::Error ModuleLoadImpl(const ModuleInfo& module);
  virtual common::Error ModuleUnLoadImpl(const ModuleInfo& module);

  IModuleConnectionClient* mclient_;
#endif
};

}  // namespace redis
}  // namespace core
}  // namespace fastonosql
