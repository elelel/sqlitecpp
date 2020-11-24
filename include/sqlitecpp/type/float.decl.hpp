#pragma once

namespace sqlsuite {
  namespace sqlite {
    namespace driver {
      template <>
      struct type_policy<float> {
        template <typename Value>
        inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
        inline static float column(sqlite3_stmt* stmt, const int i);
      };

      template <>
      struct type_policy<std::optional<float>> {
        template <typename Value>
        inline static int bind(sqlite3_stmt* stmt, const int i, Value&& value);
        inline static std::optional<float> column(sqlite3_stmt* stmt, const int i);
      };
    }
  }
}
