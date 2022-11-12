#pragma once
#include <cstdint>
#include <gpiod.h>
#include <lib/drivers/include/driver_handler.hpp>
#include <string>

#define CHIPNAME "gpiochip4"
#define ENABLE_GPIO 50
#define BUFFER_SIZE 100

class Bargraph : public DriverHandler {
public:
  void setBargraph(const std::string &value) override;

private:
  uint32_t gpio_pins[20] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                            10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

  void initGpios(struct gpiod_line_bulk *gpiod_ctx);
  void setOutput(struct gpiod_line_bulk *gpiod_ctx, const std::string &input);
  void enableDisplay();
};
