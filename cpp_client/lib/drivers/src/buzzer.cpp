#include <lib/drivers/include/buzzer.hpp>
#include <lib/drivers/include/gpioexception.hpp>
#include <string>

#define BUZZER_SCRIPT "/usr/bin/nit_buzzer_click "

void Buzzer::setBuzzer(const int8_t value) {
  std::string command = BUZZER_SCRIPT + std::to_string(value);
  if (system(command.c_str()) != 0) {
    throw GpioException("Failed to write a value to the buzzer");
  }
}
