#pragma once

#include "common.decl.hpp"

#include "../exception.decl.hpp"
#include "../exception.impl.hpp"

template <typename T>
inline static std::optional<T> sqlsuite::sqlite::driver::column_optional(sqlite3_stmt* stmt, const int i) {
  if (sqlite3_column_type(stmt, i) != SQLITE_NULL) {
    auto db = sqlite3_db_handle(stmt);
    T value = type_policy<T>::column(stmt, i);
    if (db != nullptr) {
      const auto err_code = sqlite3_extended_errcode(db);
      if (((err_code & 0xff) == SQLITE_OK) || ((err_code & 0xff) == SQLITE_ROW)) {
        return std::optional<T>(std::move(value));
      } else {
        const auto msg = "Can't get nullable value of column " + std::to_string(i);
        throw exception(db, msg.c_str(), err_code);
      }
    } else {
      const auto msg = "Can't get nullable value of column " + std::to_string(i) + ", can't get db handle from stmt";
      throw exception(msg.c_str());
    }
  }
  return std::optional<T>();
}

template <typename T>
inline static int sqlsuite::sqlite::driver::bind_optional(sqlite3_stmt* stmt, const int i, T&& value) {
  using type = typename std::decay<T>::type::value_type;
  if (value.has_value()) return type_policy<type>::bind(stmt, i, *value);
  else {
    auto sqlite_err = sqlite3_bind_null(stmt, i);
    if ((sqlite_err & 0xff) == SQLITE_OK) return sqlite_err; else {
      auto db = sqlite3_db_handle(stmt);
      if (db != nullptr) sqlite_err = sqlite3_extended_errcode(db);
      const auto msg = "Can't bind null value to column " + std::to_string(i);
      throw exception(db, msg.c_str(), sqlite_err);
    }
  }
}
