#pragma once

#pragma once
#include <gpiod.h>
#include <lib/drivers/include/driver_handler.hpp>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

class KeyClick2x2 : public DriverHandler {
public:
  struct gpio_button {
    int offset;
    char chip[20];
  };

  int get2x2Click(const int8_t button_index) override;

private:
  struct gpio_button buttons[4] = {
      {-1, "NULL"}, // Button 1 connected to analog pin. Not accessible via gpio
      {14, "gpiochip1"}, // GPIO46
      {4, "gpiochip2"},  // GPIO68
      {23, "gpiochip0"}, // GPIO23
  };

  std::vector<int> getValues();

  void printValues();
};
