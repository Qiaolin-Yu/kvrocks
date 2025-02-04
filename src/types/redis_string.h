/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "storage/redis_db.h"
#include "storage/redis_metadata.h"

struct StringPair {
  Slice key;
  Slice value;
};

enum class StringSetType { NONE, NX, XX };

struct StringSetArgs {
  uint64_t ttl;
  StringSetType type;
  bool get;
  bool keep_ttl;
};

namespace redis {

class String : public Database {
 public:
  explicit String(engine::Storage *storage, const std::string &ns) : Database(storage, ns) {}
  rocksdb::Status Append(const std::string &user_key, const std::string &value, uint64_t *new_size);
  rocksdb::Status Get(const std::string &user_key, std::string *value);
  rocksdb::Status GetEx(const std::string &user_key, std::string *value, uint64_t ttl, bool persist);
  rocksdb::Status GetSet(const std::string &user_key, const std::string &new_value,
                         std::optional<std::string> &old_value);
  rocksdb::Status GetDel(const std::string &user_key, std::string *value);
  rocksdb::Status Set(const std::string &user_key, const std::string &value);
  rocksdb::Status Set(const std::string &user_key, const std::string &value, StringSetArgs args,
                      std::optional<std::string> &ret);
  rocksdb::Status SetEX(const std::string &user_key, const std::string &value, uint64_t ttl);
  rocksdb::Status SetNX(const std::string &user_key, const std::string &value, uint64_t ttl, bool *flag);
  rocksdb::Status SetXX(const std::string &user_key, const std::string &value, uint64_t ttl, bool *flag);
  rocksdb::Status SetRange(const std::string &user_key, size_t offset, const std::string &value, uint64_t *new_size);
  rocksdb::Status IncrBy(const std::string &user_key, int64_t increment, int64_t *new_value);
  rocksdb::Status IncrByFloat(const std::string &user_key, double increment, double *new_value);
  std::vector<rocksdb::Status> MGet(const std::vector<Slice> &keys, std::vector<std::string> *values);
  rocksdb::Status MSet(const std::vector<StringPair> &pairs, uint64_t ttl = 0, bool lock = true);
  rocksdb::Status MSetNX(const std::vector<StringPair> &pairs, uint64_t ttl, bool *flag);
  rocksdb::Status CAS(const std::string &user_key, const std::string &old_value, const std::string &new_value,
                      uint64_t ttl, int *flag);
  rocksdb::Status CAD(const std::string &user_key, const std::string &value, int *flag);

 private:
  rocksdb::Status getValue(const std::string &ns_key, std::string *value);
  std::vector<rocksdb::Status> getValues(const std::vector<Slice> &ns_keys, std::vector<std::string> *values);
  rocksdb::Status getRawValue(const std::string &ns_key, std::string *raw_value);
  std::vector<rocksdb::Status> getRawValues(const std::vector<Slice> &keys, std::vector<std::string> *raw_values);
  rocksdb::Status updateRawValue(const std::string &ns_key, const std::string &raw_value);
};

}  // namespace redis
