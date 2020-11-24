cmake_minimum_required(VERSION 3.12.4 FATAL_ERROR)

find_package(SQLite3 REQUIRED)

add_library(sqlitecpp
  "api.cpp"
  "transaction.cpp"
  "exception.cpp")
set_property(TARGET sqlitecpp PROPERTY CXX_STANDARD 20)
target_compile_options (sqlitecpp PRIVATE -g -Wall -Wextra -pedantic -Werror)
target_link_libraries(sqlitecpp Catch2::Catch2 SQLite::SQLite3)
