#pragma once

#include "string.decl.hpp"

#include "common.impl.hpp"

template <typename Value>
inline int sqlite::type_policy<std::string>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  auto sqlite_err = sqlite3_bind_text(stmt, i, value.c_str(), value.size() + 1, SQLITE_TRANSIENT);
  if ((sqlite_err & 0xff) == SQLITE_OK) return sqlite_err;
  else {
    auto db = sqlite3_db_handle(stmt);
    if (db != nullptr) sqlite_err = sqlite3_extended_errcode(db);
    const auto msg = "Can't bind std::string value of " + value + " to column " + std::to_string(i);
    throw exception(db, msg.c_str(), sqlite_err);
  }
}

inline std::string sqlite::type_policy<std::string>::column(sqlite3_stmt* stmt, const int i) {
  if (sqlite3_column_type(stmt, i) != SQLITE_TEXT) {
    const auto msg = "Can't get std::string column " + std::to_string(i) + ", column is not SQLITE_TEXT";
    throw exception(msg.c_str());
  }

  const unsigned char* c = sqlite3_column_text(stmt, i);  // Force processing of length
  if (c == nullptr) {
    auto db = sqlite3_db_handle(stmt);
    if (db == nullptr) {
      const auto msg = "Can't get std::string column " + std::to_string(i) + ", can't get db handle from stmt";
      throw exception(msg.c_str());
    }
    const auto sqlite_res = sqlite3_extended_errcode(db);
    if ((sqlite_res & 0xff) != SQLITE_OK) {
      const auto msg = "Can't get std::string column " + std::to_string(i);
      throw exception(db, msg.c_str(), sqlite_res);
    } else {
      const auto msg = "Can't get std::string column " + std::to_string(i) + ", sql value is NULL";
      throw exception(db, msg.c_str(), sqlite_res);
    }
  }
    
  auto len = sqlite3_column_bytes(stmt, i);
  auto value = std::string(len - 1, 0);
  if (len > 0) memcpy(value.data(), c, len);
  return value;
}

template <typename Value>
inline int sqlite::type_policy<std::optional<std::string>>::bind(sqlite3_stmt* stmt, const int i, Value&& value) {
  return bind_optional(stmt, i, std::forward<Value>(value));
}

inline std::optional<std::string> sqlite::type_policy<std::optional<std::string>>::column(sqlite3_stmt* stmt, const int i) {
  return column_optional<std::string>(stmt, i);
}
