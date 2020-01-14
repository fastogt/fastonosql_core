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

#include <string>

#include <common/logger.h>
#include <common/patterns/singleton_pattern.h>

namespace fastonosql {
namespace core {

typedef void LogWatcher(common::logging::LOG_LEVEL level, const std::string& message, bool notify);

class Logger : public common::patterns::LazySingleton<Logger> {
  friend class common::patterns::LazySingleton<Logger>;

 public:
  void print(const char* mess, common::logging::LOG_LEVEL level, bool notify);
  void print(const std::string& mess, common::logging::LOG_LEVEL level, bool notify);

  LogWatcher* GetWatcher() const;
  void SetWathcer(LogWatcher* watcher);

 private:
  Logger();
  LogWatcher* watcher_;
};

}  // namespace core
}  // namespace fastonosql

void SET_LOG_WATCHER(fastonosql::core::LogWatcher* watcher);

void LOG_CORE_MSG(const char* mess, common::logging::LOG_LEVEL level, bool notify);
void LOG_CORE_MSG(const std::string& mess, common::logging::LOG_LEVEL level, bool notify);
