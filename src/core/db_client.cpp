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

#include <fastonosql/core/db_client.h>

namespace fastonosql {
namespace core {

NDbClient::NDbClient()
    : id_(0),
      addr_(),
      fd_(0),
      name_(),
      age_(0),
      idle_(0),
      flags_(),
      db_(0),
      sub_(0),
      psub_(0),
      multi_(0),
      qbuf_(0),
      qbuf_free_(0),
      odl_(0),
      oll_(0),
      omem_(0),
      events_(),
      cmd_() {}

void NDbClient::SetId(id_t iden) {
  id_ = iden;
}

NDbClient::id_t NDbClient::GetId() const {
  return id_;
}

void NDbClient::SetAddr(const addr_t& addr) {
  addr_ = addr;
}

NDbClient::addr_t NDbClient::GetAddr() const {
  return addr_;
}

void NDbClient::SetFd(fd_t fd) {
  fd_ = fd;
}

NDbClient::fd_t NDbClient::GetFd() const {
  return fd_;
}

void NDbClient::SetAge(age_t age) {
  age_ = age;
}

NDbClient::age_t NDbClient::GetAge() const {
  return age_;
}

void NDbClient::SetName(const name_t& name) {
  name_ = name;
}

NDbClient::name_t NDbClient::GetName() const {
  return name_;
}

void NDbClient::SetIdle(idle_t idle) {
  idle_ = idle;
}

NDbClient::age_t NDbClient::GetIdle() const {
  return idle_;
}

void NDbClient::SetFlags(const flags_t& flags) {
  flags_ = flags;
}

NDbClient::flags_t NDbClient::GetFlags() const {
  return flags_;
}

void NDbClient::SetDb(db_t db) {
  db_ = db;
}

NDbClient::db_t NDbClient::GetDb() const {
  return db_;
}

void NDbClient::SetSub(sub_t sub) {
  sub_ = sub;
}

NDbClient::sub_t NDbClient::GetSub() const {
  return sub_;
}

void NDbClient::SetPSub(psub_t psub) {
  psub_ = psub;
}

NDbClient::psub_t NDbClient::GetPSub() const {
  return psub_;
}

void NDbClient::SetMulti(multi_t multi) {
  multi_ = multi;
}

NDbClient::multi_t NDbClient::GetMulti() const {
  return multi_;
}

void NDbClient::SetQbuf(qbuf_t qbuf) {
  qbuf_ = qbuf;
}

NDbClient::qbuf_t NDbClient::GetQbuf() const {
  return qbuf_;
}

void NDbClient::SetQbufFree(qbuf_free_t qbuf_free) {
  qbuf_free_ = qbuf_free;
}

NDbClient::qbuf_free_t NDbClient::GetQbufFree() const {
  return qbuf_;
}

void NDbClient::SetOdl(odl_t odl) {
  odl_ = odl;
}

NDbClient::odl_t NDbClient::GetOdl() const {
  return odl_;
}

void NDbClient::SetOll(oll_t oll) {
  oll_ = oll;
}

NDbClient::oll_t NDbClient::GetOll() const {
  return oll_;
}

void NDbClient::SetOmem(omem_t omem) {
  omem_ = omem;
}

NDbClient::omem_t NDbClient::GetOmem() const {
  return omem_;
}

void NDbClient::SetEvents(const events_t& events) {
  events_ = events;
}

NDbClient::events_t NDbClient::GetEvents() const {
  return events_;
}

void NDbClient::SetCmd(const cmd_t& cmd) {
  cmd_ = cmd;
}

NDbClient::cmd_t NDbClient::GetCmd() const {
  return cmd_;
}

}  // namespace core
}  // namespace fastonosql
