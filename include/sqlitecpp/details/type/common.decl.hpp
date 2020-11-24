#pragma once

#include <optional>

#include "sqlite3.h"

namespace sqlite {
  template <typename T>
  inline static std::optional<T> column_optional(sqlite3_stmt* stmt, const int i);

  template <typename T>
  inline static int bind_optional(sqlite3_stmt* stmt, const int i, T&& value);

  template <typename T>
  struct type_policy {
    using return_type = typename std::remove_reference<T>::type;

    inline static return_type column(sqlite3_stmt* stmt, const int i);
  };
}
