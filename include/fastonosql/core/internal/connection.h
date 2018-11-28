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

#include <common/error.h>  // for Error

namespace fastonosql {
namespace core {
namespace internal {

template <typename H, typename C>
class Connection {
 public:
  typedef common::Optional<C> config_t;
  typedef H handle_t;

  Connection() : config_(), handle_(nullptr) {}

  ~Connection() {
    DCHECK(!IsConnected()) << "Need to be disconnected here!";
    DCHECK(!config_) << "Config should be cleared!";
    DCHECK(!handle_) << "Handle should be cleared!";
  }

  bool IsConnected() const { return IsConnected(handle_); }

  common::Error Connect(const config_t& config) WARN_UNUSED_RESULT {
    if (!config) {
      return common::make_error_inval();
    }

    if (IsConnected()) {
      return common::Error();
    }

    handle_t* handle = nullptr;
    common::Error err = Connect(*config, &handle);
    if (err) {
      return err;
    }

    config_ = config;
    handle_ = handle;
    return common::Error();
  }

  common::Error Disconnect() WARN_UNUSED_RESULT {
    if (!IsConnected()) {
      return common::Error();
    }

    common::Error err = Disconnect(&handle_);
    if (err) {
      return err;
    }

    config_ = config_t();
    handle_ = nullptr;
    return common::Error();
  }

  config_t config_;
  handle_t* handle_;

 private:
  static common::Error Connect(const C& config, handle_t** hout);  // allocate handle
  static common::Error Disconnect(handle_t** handle);              // deallocate handle
  static bool IsConnected(handle_t* handle);
};

}  // namespace internal
}  // namespace core
}  // namespace fastonosql
