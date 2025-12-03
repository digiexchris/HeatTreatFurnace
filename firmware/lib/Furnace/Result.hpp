#pragma once

#include <ostream>
#include <string>

struct Result
{
  bool success;
  std::string message;

  friend bool operator==(const Result &lhs, const Result &rhs)
  {
    return lhs.success == rhs.success;
  }

  friend bool operator!=(const Result &lhs, const Result &rhs)
  {
    return !(lhs == rhs);
  }

  operator bool() const
  {
    return success;
  }

  operator std::string() const
  {
    return message;
  }
};