#pragma once

#include "api.impl.hpp"

#include <tuple>

#include "exception.decl.hpp"

inline sqlite3* sqlite::open(const char* filename) {
  sqlite3* db;
  auto sqlite_err = sqlite3_open(filename, &db);
  if (((sqlite_err & 0xff) != SQLITE_OK) || (db == nullptr)) {
    sqlite_err = sqlite3_extended_errcode(db);
    auto msg = "Can't open database " + std::string(filename);
    throw exception(db, msg.c_str(), sqlite_err);
  }
  int fk_enabled = 0;
  sqlite_err = sqlite3_db_config(db, SQLITE_DBCONFIG_ENABLE_FKEY, 1, &fk_enabled);
  if ((sqlite_err != SQLITE_OK) || (fk_enabled != 1)) {
    auto msg = "Can't bring database into correct FOREIGN KEY mode";
    throw exception(db,  msg, sqlite_err);
  }
  return db;
}

inline sqlite3_stmt* sqlite::prepare(sqlite3* db, const char* query, const int query_sz) {
  sqlite3_stmt* stmt = nullptr;
  auto sqlite_err = sqlite3_prepare_v2(db, query, query_sz, &stmt, nullptr);
  if (((sqlite_err & 0xff) != SQLITE_OK) || (stmt == nullptr)) {
    sqlite_err = sqlite3_extended_errcode(db);
    auto msg = "Can't prepare statement " + std::string(query);
    throw exception(db, msg.c_str(), sqlite_err);
  }
  return stmt;
}

inline sqlite3_stmt* sqlite::prepare(sqlite3* db, const std::string& query) {
  return prepare(db, query.c_str(), query.size() + 1);
}

template <int I, sqlite::mapped_type Arg>
inline int sqlite::bind(sqlite3_stmt* stmt, Arg&& arg) {
  using dispatch_type = typename std::decay<Arg>::type;
  return type_policy<dispatch_type>::bind(stmt, I, std::forward<Arg>(arg));
}
        
template <int I, sqlite::mapped_type Arg, sqlite::mapped_type... Args>
inline int sqlite::bind(sqlite3_stmt* stmt, Arg&& arg, Args&&... args) {
  using dispatch_type = typename std::decay<Arg>::type;
  type_policy<dispatch_type>::bind(stmt, I, std::forward<Arg>(arg));
  return bind<I + 1>(stmt, std::forward<Args>(args)...);
}

template <sqlite::mapped_type Arg>
inline int sqlite::bind_at(sqlite3_stmt* stmt, int i, const Arg& arg) {
  using dispatch_type = typename std::decay<Arg>::type;
  return type_policy<dispatch_type>::bind(stmt, i, arg);
}

inline int sqlite::step(sqlite3_stmt* stmt) {
  auto sqlite_err = sqlite3_step(stmt);
  if (((sqlite_err &0xff) != SQLITE_ROW) && ((sqlite_err &0xff) != SQLITE_DONE)) {
    auto db = sqlite3_db_handle(stmt);
    if (db != nullptr) {
      sqlite_err = sqlite3_extended_errcode(db);
      throw exception(db, "Can't step", sqlite_err);
    } else {
      throw exception("Can't step");
    }
  }
  return sqlite_err;
}

inline int sqlite::finalize(sqlite3* db, sqlite3_stmt* stmt) {
  auto sqlite_err = sqlite3_finalize(stmt) & 0xff;
  if ((sqlite_err & 0xff) == SQLITE_OK) {
    return sqlite_err;
  }
  throw exception(db, "Finalize failed", sqlite_err);
}

inline int sqlite::close(sqlite3* db) {
  auto sqlite_err = sqlite3_close(db) & 0xff;
  if ((sqlite_err & 0xff) == SQLITE_OK) {
    return sqlite_err;
  }
  throw exception(db, "Close failed", sqlite_err);
}

inline int sqlite::changes(sqlite3* db) {
  return sqlite3_changes(db);
}

inline int64_t sqlite::last_insert_id(sqlite3* db) {
  if (changes(db) != 0) {
    auto id = sqlite3_last_insert_rowid(db);
    if (id == 0) {
      throw exception("Can't get last insert id, last rowid is 0");
    }
    return id;
  } else {
    throw exception("Can't get last insert id, number of rows affected by previous query is 0");
  }
}

inline int sqlite::transaction_begin(sqlite3* db) {
  auto sqlite_err = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  if (sqlite_err != SQLITE_OK) throw exception(db, "Can't begin transaction", sqlite_err);
  return sqlite_err;
}

inline int sqlite::transaction_commit(sqlite3* db) {
  auto sqlite_err = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
  if (sqlite_err != SQLITE_OK) throw exception(db, "Can't commit transaction", sqlite_err);
  return sqlite_err;
}

inline int sqlite::transaction_rollback(sqlite3* db) {
  auto sqlite_err = sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
  if (sqlite_err != SQLITE_OK) throw exception(db, "Can't commit transaction", sqlite_err);
  return sqlite_err;
}

template <std::size_t I, typename Arg>
inline void sqlite::columns_to(sqlite3_stmt* stmt, Arg& arg) {
  using T = std::decay_t<decltype(arg)>;
  arg = column<T>(stmt, I);
}

template <std::size_t I, typename Arg, typename... Args>
inline void sqlite::columns_to(sqlite3_stmt* stmt, Arg& arg, Args&&... args) {
  using T = std::decay_t<decltype(arg)>;
  arg = column<T>(stmt, I);
  columns_to<I+1>(stmt, std::forward<Args>(args)...);
}

template <typename ValuesTuple, std::size_t I>
inline auto sqlite::columns_as_tuple(sqlite3_stmt* stmt) {
  if constexpr (I < std::tuple_size<ValuesTuple>::value - 1) {
    using T = std::tuple_element_t<I, ValuesTuple>;
    return std::tuple_cat(std::tuple(column<T>(stmt, I)), columns_as_tuple<ValuesTuple, I + 1>(stmt));
  } else {
    using T = std::tuple_element_t<I, ValuesTuple>;
    return std::tuple(column<T>(stmt, I));
  }
}

template <typename... Args>
inline void sqlite::exec(sqlite3* db, const std::string& query, Args&&... args) {
  sqlite3_stmt* stmt = nullptr;  
  try {
    stmt = prepare(db, query);
    if constexpr (sizeof...(Args) > 0) bind(stmt, std::forward<Args>(args)...);
    step(stmt);
    finalize(db, stmt);
    stmt = nullptr;
  } catch (exception& e) {
    if (stmt != nullptr) try { finalize(db, stmt); } catch (...) {};
    throw;
  }
}

template <typename... Args>
inline int sqlite::exec_for_changes(sqlite3* db, const std::string& query, Args&&... args) {
  exec(db, query, std::forward<Args>(args)...);
  return changes(db);
}

template <typename ValuesTuple, typename... Args>
inline std::optional<ValuesTuple> sqlite::exec_for_single(sqlite3* db, const std::string& query, Args&&...args) {
  std::optional<ValuesTuple> result;
  sqlite3_stmt* stmt = nullptr;
  try {
    stmt = prepare(db, query);
    if constexpr (sizeof...(Args) > 0) bind(stmt, std::forward<Args>(args)...);
    auto step_rslt = step(stmt);
    while (step_rslt == SQLITE_ROW) {
      if (!result.has_value()) {
        result = std::move(columns_as_tuple<ValuesTuple>(stmt));
      } else throw exception("Can't execute for single row, more than one rows in result");
      step_rslt = step(stmt);
    }
    finalize(db, stmt);
    stmt = nullptr;
  } catch (...) {
    if (stmt != nullptr) try { finalize(db, stmt); } catch (...) {};
    throw;
  }
  return result;
}

template <typename ValuesTuple, std::output_iterator<ValuesTuple> OutputIterator, typename... Args>
inline void sqlite::exec_into(sqlite3* db, const std::string& query, OutputIterator output_iterator, Args&&...args) {
  sqlite3_stmt* stmt = nullptr;
  try {
    stmt = prepare(db, query);
    if constexpr (sizeof...(Args) > 0) bind(stmt, std::forward<Args>(args)...);
    auto step_rslt = step(stmt);
    while (step_rslt == SQLITE_ROW) {
      *output_iterator++ = std::move(columns_as_tuple<ValuesTuple>(stmt));
      step_rslt = step(stmt);
    }
    finalize(db, stmt);
    stmt = nullptr;
  } catch (...) {
    if (stmt != nullptr) try { finalize(db, stmt); } catch (...) {};
    throw;
  }
}

template <typename ValuesTuple, typename... Args>
inline void sqlite::exec_and_accept(sqlite3* db, const std::string& query,
                                    const std::function<void(ValuesTuple&&)>& acceptor, Args&&...args) {
  sqlite3_stmt* stmt = nullptr;
  try {
    stmt = prepare(db, query);
    if constexpr (sizeof...(Args) > 0) bind(stmt, std::forward<Args>(args)...);
    auto step_rslt = step(stmt);
    while (step_rslt == SQLITE_ROW) {
      acceptor(std::move(columns_as_tuple<ValuesTuple>(stmt)));
      step_rslt = step(stmt);
    }
    finalize(db, stmt);
    stmt = nullptr;
  } catch (...) {
    if (stmt != nullptr) try { finalize(db, stmt); } catch (...) {};
    throw;
  }
}

