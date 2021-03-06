/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

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

#include <fastonosql/core/icommand_translator_base.h>

namespace fastonosql {
namespace core {
namespace memcached {

class CommandTranslator : public ICommandTranslatorBase {
 public:
  explicit CommandTranslator(const std::vector<CommandHolder>& commands);
  const char* GetDBName() const override;

 private:
  common::Error CreateKeyCommandImpl(const NDbKValue& key, command_buffer_t* cmdstring) const override;
  common::Error LoadKeyCommandImpl(const NKey& key,
                                   common::Value::Type type,
                                   command_buffer_t* cmdstring) const override;
  common::Error DeleteKeyCommandImpl(const NKey& key, command_buffer_t* cmdstring) const override;
  common::Error RenameKeyCommandImpl(const NKey& key,
                                     const nkey_t& new_name,
                                     command_buffer_t* cmdstring) const override;

  bool IsLoadKeyCommandImpl(const CommandInfo& cmd) const override;

  common::Error ChangeKeyTTLCommandImpl(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const override;
  common::Error LoadKeyTTLCommandImpl(const NKey& key, command_buffer_t* cmdstring) const override;
};

}  // namespace memcached
}  // namespace core
}  // namespace fastonosql
