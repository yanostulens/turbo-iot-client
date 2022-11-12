#pragma once

#include <iostream>

class FrequencyException : public std::exception {
public:
  FrequencyException(const std::string &msg) : message(msg) {}

  virtual const char *what() const throw();

private:
  std::string message;
};
