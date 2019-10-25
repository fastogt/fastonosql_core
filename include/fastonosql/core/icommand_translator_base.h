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

#pragma once

#include <string>
#include <vector>

#include <fastonosql/core/icommand_translator.h>

namespace fastonosql {
namespace core {

class ICommandTranslatorBase : public ICommandTranslator {
 public:
  explicit ICommandTranslatorBase(const std::vector<CommandHolder>& commands);
  ~ICommandTranslatorBase() override;

  const char* GetDBName() const override = 0;

 private:
  common::Error ChangeKeyTTLCommandImpl(const NKey& key, ttl_t ttl, command_buffer_t* cmdstring) const override;
  common::Error LoadKeyTTLCommandImpl(const NKey& key, command_buffer_t* cmdstring) const override;
  common::Error PublishCommandImpl(const trans_ps_channel_t& channel,
                                   const std::string& message,
                                   command_buffer_t* cmdstring) const override;
  common::Error SubscribeCommandImpl(const trans_ps_channel_t& channel, command_buffer_t* cmdstring) const override;
};

}  // namespace core
}  // namespace fastonosql
