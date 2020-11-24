#pragma once

#include "api.decl.hpp"

#include <functional>

namespace sqlite {
  // Execute std::function in a sqlite transaction
  
  template <typename Result>
  struct transaction {
    inline transaction(sqlite3* db, std::function<Result()> f) :
      db_(db),
      f_(f),
      need_close_(false) {} ;

    inline ~transaction() {
      if (need_close_) {
        try { transaction_rollback(db_); } catch (...) {}              
      }
    }
          
    inline Result operator*() {
      transaction_begin(db_);
      need_close_ = true;
      Result result = f_();
      transaction_commit(db_);
      need_close_ = false;
      return result;
    }
          
  private:
    sqlite3* db_;
    const std::function<Result()> f_;
    bool need_close_;
  };

  struct transaction_void {
    inline transaction_void(sqlite3* db, std::function<void()> f) :
      db_(db),
      f_(f) {} ;

    inline void operator*() const {
      transaction_begin(db_);
      try {
        f_();
        transaction_commit(db_);
      } catch (...) {
        try { transaction_rollback(db_); } catch (...) {}
        throw;
      }
    }
  private:
    sqlite3* db_;
    const std::function<void()> f_;
  };
}

