#include <lib/drivers/include/frequency_exception.hpp>

const char *FrequencyException::what() const throw() {
  return this->message.c_str();
}
