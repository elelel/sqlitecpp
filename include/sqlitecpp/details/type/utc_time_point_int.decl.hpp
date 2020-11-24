#pragma once

#include <chrono>

namespace sqlite {
  template <>
  struct type_policy<std::chrono::time_point<std::chrono::utc_clock>> {
    using time_point = std::chrono::time_point<std::chrono::utc_clock>;
        
    template <typename Value> 
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static time_point column(sqlite3_stmt* stmt, const int i);
  };
        
  template <>
  struct type_policy<std::optional<std::chrono::time_point<std::chrono::utc_clock>>> {
    using time_point = std::chrono::time_point<std::chrono::utc_clock>;
        
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::optional<time_point> column(sqlite3_stmt* stmt, const int i);
  };
}

