#pragma once

#include <exception>

#include "sqlite3.h"

namespace sqlsuite {
  namespace sqlite {
    namespace driver {
      struct exception : std::exception {
        exception() noexcept;
        exception(const char* msg) noexcept;
        exception(const std::string& msg) noexcept;
        exception(sqlite3* db, const char* msg, const int err_code) noexcept;
        exception(const exception& other) noexcept;
          
        virtual const char* what() const noexcept override;
        exception& operator=(const exception& other) noexcept;

        int err_code() const noexcept;
        int primary_err_code() const noexcept;

        const char* msg() const noexcept;
        const char* sqlite_err_msg() const noexcept;
      private:
        int err_code_;
        const char *sqlite_err_msg_;
        char msg_[1024];
        char what_[2048];
      };
    }
  }
}
