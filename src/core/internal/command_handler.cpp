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

#include <fastonosql/core/internal/command_handler.h>

#include <fastonosql/core/command_holder.h>

namespace fastonosql {
namespace core {
namespace internal {

CommandHandler::CommandHandler(ICommandTranslator* translator) : translator_(translator) {}

common::Error CommandHandler::Execute(const command_buffer_t& command, FastoObject* out) {
  commands_args_t standart_argv;
  if (!ParseCommandLine(command, &standart_argv)) {
    return common::make_error_inval();
  }

  return Execute(standart_argv, out);
}

common::Error CommandHandler::Execute(commands_args_t argv, FastoObject* out) {
  const CommandHolder* cmd = nullptr;
  size_t off = 0;
  common::Error err = translator_->TestCommandLineArgs(argv, &cmd, &off);
  if (err) {
    return err;
  }

  commands_args_t stabled;
  for (size_t i = off; i < argv.size(); ++i) {
    stabled.push_back(argv[i]);
  }
  return cmd->func_(this, stabled, out);
}

}  // namespace internal
}  // namespace core
}  // namespace fastonosql
