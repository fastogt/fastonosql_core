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
#include <vector>

#include <common/value.h>

namespace fastonosql {
namespace core {

class StreamValue : public common::Value {
 public:
  struct Entry {
    bool Equals(const Entry& other) const;

    std::string name;
    std::string value;
  };
  typedef std::string stream_id;
  struct Stream {
    bool Equals(const Stream& other) const;

    stream_id sid;
    std::vector<Entry> entries;
  };
  typedef std::vector<Stream> streams_t;

  static const common::Value::Type TYPE_STREAM = static_cast<common::Value::Type>(common::Value::USER_TYPES + 1);
  StreamValue();
  ~StreamValue() override;

  bool GetAsString(std::string* out_value) const override WARN_UNUSED_RESULT;
  StreamValue* DeepCopy() const override;
  bool Equals(const Value* other) const override;

  streams_t GetStreams() const;
  void SetStreams(const streams_t& streams);

 private:
  streams_t streams_;
  DISALLOW_COPY_AND_ASSIGN(StreamValue);
};

inline bool operator==(const StreamValue::Entry& r, const StreamValue::Entry& l) {
  return r.Equals(l);
}

inline bool operator!=(const StreamValue::Entry& r, const StreamValue::Entry& l) {
  return !(r == l);
}

inline bool operator==(const StreamValue::Stream& r, const StreamValue::Stream& l) {
  return r.Equals(l);
}

inline bool operator!=(const StreamValue::Stream& r, const StreamValue::Stream& l) {
  return !(r == l);
}

class JsonValue : public common::Value {  // simple json class value, only save
                                          // string without validation
 public:
  static const common::Value::Type TYPE_JSON = static_cast<common::Value::Type>(common::Value::USER_TYPES + 2);
  explicit JsonValue(const std::string& json_value);
  ~JsonValue() override;

  bool GetAsString(std::string* out_value) const override WARN_UNUSED_RESULT;
  JsonValue* DeepCopy() const override;
  bool Equals(const Value* other) const override;

  static bool IsValidJson(const std::string& json);

 private:
  std::string value_;
  DISALLOW_COPY_AND_ASSIGN(JsonValue);
};

class GraphValue : public common::Value {
 public:
  static const common::Value::Type TYPE_GRAPH = static_cast<common::Value::Type>(common::Value::USER_TYPES + 3);
  GraphValue();
  ~GraphValue() override;

  GraphValue* DeepCopy() const override;
  bool Equals(const Value* other) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(GraphValue);
};

class BloomValue : public common::Value {
 public:
  static const common::Value::Type TYPE_BLOOM = static_cast<common::Value::Type>(common::Value::USER_TYPES + 4);
  BloomValue();
  ~BloomValue() override;

  BloomValue* DeepCopy() const override;
  bool Equals(const Value* other) const override;

 private:
  DISALLOW_COPY_AND_ASSIGN(BloomValue);
};

class SearchValue : public common::Value {
 public:
  static const common::Value::Type TYPE_FT_INDEX = static_cast<common::Value::Type>(common::Value::USER_TYPES + 5);
  static const common::Value::Type TYPE_FT_TERM = static_cast<common::Value::Type>(common::Value::USER_TYPES + 6);
  ~SearchValue() override;

  static SearchValue* CreateSearchIndex();
  static SearchValue* CreateSearchDocument();

  SearchValue* DeepCopy() const override;
  bool Equals(const Value* other) const override;

 private:
  explicit SearchValue(common::Value::Type type);

  DISALLOW_COPY_AND_ASSIGN(SearchValue);
};

common::Value* CreateEmptyValueFromType(common::Value::Type value_type);
const char* GetTypeName(common::Value::Type value_type);

std::string ConvertValue(common::Value* value, const std::string& delimiter);
std::string ConvertValue(common::ArrayValue* array, const std::string& delimiter);
std::string ConvertValue(common::SetValue* set, const std::string& delimiter);
std::string ConvertValue(common::ZSetValue* zset, const std::string& delimiter);
std::string ConvertValue(common::HashValue* hash, const std::string& delimiter);
std::string ConvertValue(common::FundamentalValue* value, const std::string& delimiter);
std::string ConvertValue(common::StringValue* value, const std::string& delimiter);
std::string ConvertValue(common::ByteArrayValue* value, const std::string& delimiter);
std::string ConvertValue(StreamValue* value, const std::string& delimiter);
// extended
std::string ConvertValue(JsonValue* value, const std::string& delimiter);
std::string ConvertValue(GraphValue* value, const std::string& delimiter);
std::string ConvertValue(BloomValue* value, const std::string& delimiter);
std::string ConvertValue(SearchValue* value, const std::string& delimiter);
}  // namespace core
}  // namespace fastonosql
