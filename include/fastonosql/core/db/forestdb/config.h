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

#include <string>

#include <fastonosql/core/config/config.h>

namespace fastonosql {
namespace core {
namespace forestdb {

struct Config : public LocalConfig {
  typedef LocalConfig base_class;

  Config();

  void Init(const config_args_t& args);
  config_args_t ToArgs() const;

  std::string db_name;
};

}  // namespace forestdb
}  // namespace core
}  // namespace fastonosql
