#pragma once

namespace sqlite {
  template <>
  struct type_policy<std::string> {
    template <typename Value> 
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::string column(sqlite3_stmt* stmt, const int i);
  };
        
  template <>
  struct type_policy<std::optional<std::string>> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::optional<std::string> column(sqlite3_stmt* stmt, const int i);
  };
}
