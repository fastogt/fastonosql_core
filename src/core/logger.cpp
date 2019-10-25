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

#include <fastonosql/core/logger.h>

#include <common/logger.h>

namespace fastonosql {
namespace core {

Logger::Logger() : watcher_(nullptr) {}

void Logger::print(const char* mess, common::logging::LOG_LEVEL level, bool notify) {
  print(std::string(mess), level, notify);
}

void Logger::print(const std::string& mess, common::logging::LOG_LEVEL level, bool notify) {
  RUNTIME_LOG(level) << mess;
  if (watcher_) {
    watcher_(level, mess, notify);
  }
}

LogWatcher* Logger::GetWatcher() const {
  return watcher_;
}

void Logger::SetWathcer(LogWatcher* watcher) {
  watcher_ = watcher;
}

}  // namespace core
}  // namespace fastonosql

void SET_LOG_WATCHER(fastonosql::core::LogWatcher* watcher) {
  fastonosql::core::Logger::GetInstance().SetWathcer(watcher);
}

void LOG_CORE_MSG(const char* mess, common::logging::LOG_LEVEL level, bool notify) {
  return fastonosql::core::Logger::GetInstance().print(mess, level, notify);
}

void LOG_CORE_MSG(const std::string& mess, common::logging::LOG_LEVEL level, bool notify) {
  return fastonosql::core::Logger::GetInstance().print(mess, level, notify);
}
