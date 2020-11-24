#pragma once

#include "common.decl.hpp"

namespace sqlite {
  template <>
  struct type_policy<bool> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static bool column(sqlite3_stmt* stmt, const int i);
  };

  template <>
  struct type_policy<std::optional<bool>> {
    template <typename Value>
    inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
    inline static std::optional<bool> column(sqlite3_stmt* stmt, const int i);
  };
}
