#pragma once

namespace sqlite {
  template <>
  struct type_policy<int> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static int column(sqlite3_stmt* stmt, const int i);
  };

  template <>
  struct type_policy<std::optional<int>> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::optional<int> column(sqlite3_stmt* stmt, const int i);
  };
}

