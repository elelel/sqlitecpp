#pragma once

#include "sqlite3.h"

#include "type/common.decl.hpp"

namespace sqlite {
  template <typename Type>
  concept mapped_type = requires {
    typename type_policy<Type>;

    // TODO: 
    // std::is_same<int, decltype(Type::bind(sqlite3_stmt{}, int{}, std::decay_t<Type>::value_type{}))>;
    //std::is_same<std::decay_t<Type>::value_type, decltype(Type::column(sqlite3_stmt& stmt, const int))>;
    // std::is_void<decltype(Type::emplace(sqlite3_stmt&, const int, std::decay_t<Type>::value_type&))>;
  };
}

