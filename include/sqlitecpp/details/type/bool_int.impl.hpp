#pragma once

#include "bool_int.decl.hpp"

#include "common.impl.hpp"

template <typename Value>
inline int sqlite::type_policy<bool>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  auto sqlite_err = sqlite3_bind_int(stmt, i, value ? 1 : 0);
  if ((sqlite_err & 0xff) == SQLITE_OK) return sqlite_err; else {
    auto db = sqlite3_db_handle(stmt);
    if (db != nullptr) sqlite_err = sqlite3_extended_errcode(db);
    const auto msg = "Can't bind bool value of " + std::to_string(value) + " to column " + std::to_string(i);
    throw exception(db, msg.c_str(), sqlite_err);
  }
}

inline bool sqlite::type_policy<bool>::column(sqlite3_stmt* stmt, const int i) {
  auto column_type = sqlite3_column_type(stmt, i);
  if (column_type != SQLITE_INTEGER) {
    const auto msg = "Can't get bool column " + std::to_string(i) + ", column type is " +std::to_string(column_type) + " instead of SQLITE_INTEGER";
    throw exception(msg.c_str());
  }
  return sqlite3_column_int(stmt, i) != 0;
}

template <typename Value>
inline int sqlite::type_policy<std::optional<bool>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  return bind_optional(stmt, i, std::forward<Value>(value));
}

inline std::optional<bool> sqlite::type_policy<std::optional<bool>>::column(sqlite3_stmt* stmt, const int i) {
  return column_optional<bool>(stmt, i);
}
