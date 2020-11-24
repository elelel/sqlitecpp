#pragma once

#include "int.decl.hpp"

#include "common.impl.hpp"

template <typename Value>
inline int sqlite::type_policy<std::vector<int64_t>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  auto sqlite_err = sqlite3_bind_blob64(stmt, i, value.data(), SQLITE_TRANSIENT);
  if ((sqlite_err & 0xff) == SQLITE_OK) return sqlite_err; else {
    auto db = sqlite3_db_handle(stmt);
    if (db != nullptr) sqlite_err = sqlite3_extended_errcode(db);
    const auto msg = "Can't bind int value of " + std::to_string(value) + " to column " + std::to_string(i);
    throw exception(db, msg.c_str(), sqlite_err);
  }
}

inline std::vector<int64_t> sqlite::type_policy<std::vector<int64_t>>::column(sqlite3_stmt* stmt, const int i) {
  auto column_type = sqlite3_column_type(stmt, i);
  if (column_type != SQLITE_BLOB) {
    const auto msg = "Can't get int column " + std::to_string(i) + ", column type is " + std::to_string(column_type) + " instead of SQLITE_BLOB";
    throw exception(msg.c_str());
  }

  const void* b = sqlite3_column_blob(stmt, i);
  if (b == nullptr) {
    auto db = sqlite3_db_handle(stmt);
    if (db == nullptr) {
      const auto msg = "Can't get std::vector column " + std::to_string(i) + ", can't get db handle from stmt";
      throw exception(msg.c_str());
    }
    const auto sqlite_res = sqlite3_extended_errcode(db);
    if ((sqlite_res & 0xff) != SQLITE_OK) {
      const auto msg = "Can't get std::vector column " + std::to_string(i);
      throw exception(db, msg.c_str(), sqlite_res);
    } else {
      const auto msg = "Can't get std::vector column " + std::to_string(i) + ", sql value is NULL";
      throw exception(db, msg.c_str(), sqlite_res);
    }
  }

  auto len = sqlite3_column_bytes(stmt, i);
  std::vector<int64_t> value;
  if (len > 0) {
    value.resize(len);
    memcpy(value.data(), b, len);
  }
  return value;
}

template <typename Value>
inline int sqlite::type_policy<std::optional<std::vector<int64_t>>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  return bind_optional(stmt, i, std::forward<Value>(value));
}

inline std::optional<std::vector<int64_t>> sqlite::type_policy<std::optional<std::vector<int64_t>>>::column(sqlite3_stmt* stmt, const int i) {
  return column_optional<std::vector<int64_t>>(stmt, i);
}
