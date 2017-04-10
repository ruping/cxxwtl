// -*- mode: c++; coding: utf-8 -*-
#pragma once
#ifndef WTL_EXCEPTION_HPP_
#define WTL_EXCEPTION_HPP_

#include <stdexcept>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
namespace wtl {
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

class ExitSuccess: public std::runtime_error {
  public:
    ExitSuccess(const char* msg="EXIT_SUCCESS"): std::runtime_error(msg) {}
};

class KeyboardInterrupt: public std::runtime_error {
  public:
    KeyboardInterrupt(const char* msg="KeyboardInterrupt"): std::runtime_error(msg) {}
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////
} // namespace wtl
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////

#endif // WTL_EXCEPTION_HPP_
