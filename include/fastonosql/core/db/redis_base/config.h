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

#include <fastonosql/core/db/redis_compatible/config.h>

namespace fastonosql {
namespace core {
namespace redis_base {

struct Config : public redis_compatible::Config {
  typedef redis_compatible::Config base_class;
  explicit Config(const common::net::HostAndPort& host);

  bool Equals(const Config& other) const;
};

inline bool operator==(const Config& r, const Config& l) {
  return r.Equals(l);
}

inline bool operator!=(const Config& r, const Config& l) {
  return !(r == l);
}

}  // namespace redis_base
}  // namespace core
}  // namespace fastonosql
