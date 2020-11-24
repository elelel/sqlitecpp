#pragma once

#include "common.impl.hpp"

template <typename Value>
inline int sqlite::type_policy<std::chrono::time_point<std::chrono::seconds>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  auto unix_time = std::chrono::duration_cast<std::chrono::seconds>(value.time_since_epoch()).count();
  auto sqlite_err = sqlite3_bind_int(stmt, i, unix_time);
  if ((sqlite_err & 0xff) == SQLITE_OK) return sqlite_err; else {
    auto db = sqlite3_db_handle(stmt);
    if (db != nullptr) sqlite_err = sqlite3_extended_errcode(db);
    const auto msg = "Can't bind time_point value of " + std::to_string(unix_time) + " to int column " + std::to_string(i);
    throw exception(db, msg.c_str(), sqlite_err);
  }
}

inline auto sqlite::type_policy<std::chrono::time_point<std::chrono::seconds>>::column(sqlite3_stmt* stmt, const int i) -> time_point {
  auto column_type = sqlite3_column_type(stmt, i);
  if (column_type != SQLITE_INTEGER) {
    const auto msg = "Can't get time_point column " + std::to_string(i) + ", column type is " +std::to_string(column_type) + " instead of SQLITE_INTEGER";
    throw exception(msg.c_str());
  }
  auto unix_time = sqlite3_column_int(stmt, i);
  return time_point(std::chrono::seconds(unix_time));
}

template <typename Value>
inline int sqlite::type_policy<std::optional<std::chrono::time_point<std::chrono::seconds>>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  return bind_optional(stmt, i, std::forward<Value>(value));
}
  
inline auto sqlite::type_policy<std::optional<std::chrono::time_point<std::chrono::seconds>>>::column(sqlite3_stmt* stmt, const int i) -> std::optional<time_point> {
  return column_optional<time_point>(stmt, i);
}

