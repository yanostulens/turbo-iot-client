#include <lib/drivers/include/gpioexception.hpp>

const char *GpioException::what() const throw() {
  return this->message.c_str();
}
