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

#include <fastonosql/core/ssh_info.h>

#include <common/convert2string.h>
#include <common/file_system/types.h>

#include <fastonosql/core/macros.h>

#define DEFAULT_SSH_PORT 22
#define DEFAULT_PUB_KEY_PATH "~/.ssh/id_rsa.pub"
#define DEFAULT_PRIVATE_KEY_PATH "~/.ssh/id_rsa"

#define HOST_FIELD "host"
#define USER_FIELD "user"
#define PASSWORD_FIELD "password"
#define PUBKEY_FIELD "public_key"
#define PRIVKEY_FIELD "private_key"
#define USE_PUBLICKEY_FIELD "use_public_key"
#define PASSPHRASE_FIELD "passphrase"
#define CURMETHOD_FIELD "current_method"

namespace common {

std::string ConvertToString(const fastonosql::core::SSHInfo& info) {
  return info.ToString();
}

bool ConvertFromString(const std::string& from, fastonosql::core::SSHInfo* out) {
  if (from.empty() || !out) {
    return false;
  }

  fastonosql::core::SSHInfo info;
  fastonosql::core::PublicPrivate key;
  size_t pos = 0;
  size_t start = 0;
  while ((pos = from.find(MARKER_STR, start)) != std::string::npos) {
    std::string line = from.substr(start, pos - start);
    size_t delem = line.find_first_of(COLON_CHAR);
    if (delem != std::string::npos) {
      std::string field = line.substr(0, delem);
      std::string value = line.substr(delem + 1);
      if (field == HOST_FIELD) {
        common::net::HostAndPort lhost;
        if (common::ConvertFromString(value, &lhost)) {
          info.SetHost(lhost);
        }
      } else if (field == USER_FIELD) {
        info.SetUsername(value);
      } else if (field == PASSWORD_FIELD) {
        info.SetPassword(value);
      } else if (field == PUBKEY_FIELD) {
        key.public_key_path = value;
      } else if (field == PRIVKEY_FIELD) {
        key.private_key_path = value;
      } else if (field == USE_PUBLICKEY_FIELD) {
        bool val;
        if (common::ConvertFromString(value, &val)) {
          key.use_public_key = val;
        }
      } else if (field == PASSPHRASE_FIELD) {
        info.SetPassPharse(value);
      } else if (field == CURMETHOD_FIELD) {
        uint8_t lcurrent_method;
        if (common::ConvertFromString(value, &lcurrent_method)) {
          info.SetAuthMethod(static_cast<fastonosql::core::SSHInfo::AuthenticationMethod>(lcurrent_method));
        }
      }
    }
    start = pos + sizeof(MARKER_STR) - 1;
  }

  info.SetKey(key);
  *out = info;
  return true;
}

}  // namespace common

namespace fastonosql {
namespace core {

PublicPrivate::PublicPrivate()
    : PublicPrivate(common::file_system::prepare_path(DEFAULT_PUB_KEY_PATH),
                    common::file_system::prepare_path(DEFAULT_PRIVATE_KEY_PATH),
                    true) {}

PublicPrivate::PublicPrivate(const std::string& public_key, const std::string& private_key, bool use_public_key)
    : public_key_path(public_key), private_key_path(private_key), use_public_key(use_public_key) {}

bool PublicPrivate::IsValid() const {
  return !private_key_path.empty();
}

SSHInfo::SSHInfo()
    : host_(common::net::HostAndPort::CreateLocalHost(DEFAULT_SSH_PORT)),
      username_(),
      key_(),
      current_method_(UNKNOWN),
      password_(),
      runtime_password_() {}

SSHInfo::SSHInfo(const common::net::HostAndPort& host,
                 const std::string& username,
                 const std::string& password,
                 const PublicPrivate& key,
                 const std::string& passphrase,
                 AuthenticationMethod method)
    : host_(host),
      username_(username),
      passphrase_(passphrase),
      key_(key),
      current_method_(method),
      password_(),
      runtime_password_() {
  SetPassword(password);
}

bool SSHInfo::IsValid() const {
  if (current_method_ == PASSWORD) {
    return host_.IsValid() && !username_.empty() && !password_.empty();
  } else if (current_method_ == PUBLICKEY) {
    return host_.IsValid() && !username_.empty() && key_.IsValid();
  } else if (current_method_ == ASK_PASSWORD) {
    return host_.IsValid() && !username_.empty() && password_.empty();
  }

  return false;
}

std::string SSHInfo::ToString() const {
  std::stringstream str;
  str << CURMETHOD_FIELD COLON_STR << current_method_ << MARKER_STR << HOST_FIELD COLON_STR
      << common::ConvertToString(host_) << MARKER_STR USER_FIELD COLON_STR << username_
      << MARKER_STR PASSWORD_FIELD COLON_STR << GetPassword() << MARKER_STR PUBKEY_FIELD COLON_STR
      << key_.public_key_path << MARKER_STR PRIVKEY_FIELD COLON_STR << key_.private_key_path
      << MARKER_STR USE_PUBLICKEY_FIELD COLON_STR << common::ConvertToString(key_.use_public_key)
      << MARKER_STR PASSPHRASE_FIELD COLON_STR << passphrase_ << MARKER_STR;
  return str.str();
}

SSHInfo::AuthenticationMethod SSHInfo::GetAuthMethod() const {
  return current_method_;
}

void SSHInfo::SetAuthMethod(AuthenticationMethod auth) {
  current_method_ = auth;
}

std::string SSHInfo::GetPassPharse() const {
  return passphrase_;
}

void SSHInfo::SetPassPharse(const std::string& phrase) {
  passphrase_ = phrase;
}

common::net::HostAndPort SSHInfo::GetHost() const {
  return host_;
}

void SSHInfo::SetHost(const common::net::HostAndPort& host) {
  host_ = host;
}

PublicPrivate SSHInfo::GetKey() const {
  return key_;
}

void SSHInfo::SetKey(const PublicPrivate& key) {
  key_ = key;
}

std::string SSHInfo::GetUsername() const {
  return username_;
}

void SSHInfo::SetUsername(const std::string& name) {
  username_ = name;
}

std::string SSHInfo::GetRuntimePassword() const {
  return runtime_password_;
}

void SSHInfo::SetPassword(const std::string& password) {
  runtime_password_ = password;
  if (current_method_ == ASK_PASSWORD) {
    password_ = std::string();
  } else {
    password_ = password;
  }
}

bool SSHInfo::Equals(const SSHInfo& other) const {
  return host_ == other.host_ && GetPassword() == other.GetPassword() && passphrase_ == other.passphrase_ &&
         username_ == other.username_ && current_method_ == other.current_method_ && key_ == other.key_;
}

std::string SSHInfo::GetPassword() const {
  return password_;
}

}  // namespace core
}  // namespace fastonosql
