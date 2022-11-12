#pragma once

#include <iostream>

class GpioException : public std::exception {
public:
  GpioException(const std::string &msg) : message(msg) {}

  virtual const char *what() const throw();

private:
  std::string message;
};
