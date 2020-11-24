#pragma once

#include "exception.decl.hpp"

#include <cstring>
#include <stdio.h>

sqlite::exception::exception() noexcept :
  exception(nullptr, nullptr, SQLITE_ERROR) {
}

sqlite::exception::exception(const char* msg) noexcept :
  exception(nullptr, msg, SQLITE_ERROR) {
}

sqlite::exception::exception(const std::string& msg) noexcept :
  exception(nullptr, msg.c_str(), SQLITE_ERROR) {
}

sqlite::exception::exception(sqlite3* db, const char* msg, const int err_code) noexcept :
  err_code_(err_code) {
  if (msg != nullptr) {
    snprintf(msg_, sizeof(msg_), "%s", msg);
  } else {
    snprintf(msg_, sizeof(msg_), "unknown sqlite exception");
  }
  if ((db != nullptr) && (err_code != SQLITE_OK)) {
    sqlite_err_msg_ = sqlite3_errmsg(db);
  } else {
    sqlite_err_msg_ = nullptr;
  }

  if ((msg_[0] != 0) && (sqlite_err_msg_ != nullptr) && (sqlite_err_msg_[0] != 0)) {
    snprintf(what_, sizeof(what_), "%s: %s", msg_, sqlite_err_msg_);
  } else if (msg_[0] != 0) {
    snprintf(what_, sizeof(what_), "%s", msg_);
  } else if (sqlite_err_msg_[0] != 0) {
    snprintf(what_, sizeof(what_), "%s", sqlite_err_msg_);
  } else {
    what_[0] = 0;
  }
}

sqlite::exception::exception(const exception& other) noexcept :
  err_code_(other.err_code_),
  sqlite_err_msg_(other.sqlite_err_msg_) {
  memcpy(what_, other.what_, sizeof(what_));
}

const char* sqlite::exception::what() const noexcept {
  return what_;
}

const char* sqlite::exception::msg() const noexcept {
  return msg_;
}

auto sqlite::exception::operator=(const exception& other) noexcept -> exception&
  {
   err_code_ = other.err_code_;
   sqlite_err_msg_ = other.sqlite_err_msg_;
   memcpy(what_, other.what_, sizeof(what_));
   return *this;
}


int sqlite::exception::err_code() const noexcept {
  return err_code_;
}

int sqlite::exception::primary_err_code() const noexcept {
  return err_code_ & 0xff;
}
  
const char* sqlite::exception::sqlite_err_msg() const noexcept {
  return sqlite_err_msg_;
}
