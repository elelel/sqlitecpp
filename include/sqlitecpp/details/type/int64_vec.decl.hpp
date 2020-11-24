#pragma once

#include <vector>

namespace sqlite {
  template <>
  struct type_policy<std::vector<int64_t>> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::vector<int64_t> column(sqlite3_stmt* stmt, const int i);
  };

  template <>
  struct type_policy<std::optional<std::vector<int64_t>>> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::optional<std::vector<int64_t>> column(sqlite3_stmt* stmt, const int i);
  };
}

