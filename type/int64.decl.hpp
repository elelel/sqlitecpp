#pragma once

#include "common.decl.hpp"

namespace sqlsuite {
  namespace sqlite {
    namespace driver {
      template <>
      struct type_policy<int64_t> {
        template <typename Value>
        inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
        inline static int64_t column(sqlite3_stmt* stmt, const int i);
      };

      template <>
      struct type_policy<std::optional<int64_t>> {
        template <typename Value>
        inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
        inline static std::optional<int64_t> column(sqlite3_stmt* stmt, const int i);
      };
    }
  }
}
