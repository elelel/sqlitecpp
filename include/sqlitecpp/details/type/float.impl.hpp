#pragma once

#include "int64.decl.hpp"

#include "common.impl.hpp"

template <typename Value>
inline int sqlite::type_policy<float>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  auto sqlite_err = sqlite3_bind_double(stmt, i, std::forward<Value>(value));
  if ((sqlite_err & 0xff) == SQLITE_OK) return sqlite_err; else {
    auto db = sqlite3_db_handle(stmt);
    if (db != nullptr) sqlite_err = sqlite3_extended_errcode(db);
    const auto msg = "Can't bind float value of " + std::to_string(value) + " to column " + std::to_string(i);
    throw exception(db, msg.c_str(), sqlite_err);
  }
}

inline float sqlite::type_policy<float>::column(sqlite3_stmt* stmt, const int i) {
  auto column_type = sqlite3_column_type(stmt, i);
  if (column_type != SQLITE_FLOAT) {
    const auto msg = "Can't get float column " + std::to_string(i) + ", column type is " +std::to_string(column_type) + " instead of SQLITE_FLOAT";
    throw exception(msg.c_str());
  }
  auto value = sqlite3_column_double(stmt, i);
  return std::move(value);
}

template <typename Value>
inline int sqlite::type_policy<std::optional<float>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  return bind_optional(stmt, i, std::forward<Value>(value));
}

inline std::optional<float> sqlite::type_policy<std::optional<float>>::column(sqlite3_stmt* stmt, const int i) {
  return column_optional<float>(stmt, i);
}
