#pragma once

#include <iterator>

#include "sqlite3.h"

#include "concepts.hpp"

namespace sqlite {

  sqlite3* open(const char* filename);
        
  sqlite3_stmt* prepare(sqlite3* db, const char* query, const int query_sz = -1);
  sqlite3_stmt* prepare(sqlite3* db, const std::string& query);
        
  template <int I = 1, mapped_type Arg>
  inline int bind(sqlite3_stmt* stmt, Arg&& arg);
        
  template <int I = 1, mapped_type Arg, mapped_type... Args>
  inline int bind(sqlite3_stmt* stmt, Arg&& arg, Args&&... args);

  template <mapped_type Arg>
  inline int bind_at(sqlite3_stmt* stmt, int i, const Arg& arg);

  template <std::size_t I = 0, typename Arg>
  inline void columns_to(sqlite3_stmt* stmt, Arg& arg);

  template <std::size_t I = 0, typename Arg, typename... Args>
  inline void columns_to(sqlite3_stmt* stmt, Arg& arg, Args&&... args);

  template <typename ValuesTuple, std::size_t I = 0>
  inline auto columns_as_tuple(sqlite3_stmt* stmt);

  int step(sqlite3_stmt* stmt);

  template <typename T>
  inline T column(sqlite3_stmt* stmt, int i) {
    using dispatch_type = typename std::decay<T>::type;
    return type_policy<dispatch_type>::column(stmt, i);
  }

  int finalize(sqlite3_stmt** stmt);
  int finalize(sqlite3_stmt* stmt);

  int close(sqlite3** db);
  int close(sqlite3* db);

  int changes(sqlite3* db);

  int64_t last_insert_id(sqlite3* db);

  int transaction_begin(sqlite3* db);
  int transaction_commit(sqlite3* db);
  int transaction_rollback(sqlite3* db);

  template <typename... Args>
  inline void exec(sqlite3* db, const std::string& query, Args&&...args);
  
  // INSERT, UPDATE, DELETE only
  template <typename ValuesTuple, typename... Args>
  inline int exec_for_changes(sqlite3* db, const std::string& query, Args&&...args);

  template <typename ValuesTuple, typename... Args>
  inline std::optional<ValuesTuple> exec_for_single(sqlite3* db, const std::string& query, Args&&...args);

  // Outputs results into output iterator which must have value_type of row tuple
  template <std::output_iterator OutputIterator, typename... Args>
  inline void exec_into(sqlite3* db, const std::string& query, OutputIterator output_iterator, Args&&...args);

}
