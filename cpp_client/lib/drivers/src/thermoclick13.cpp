#include <cstdint>
#include <fcntl.h>
#include <gpiod.h>
#include <lib/drivers/include/gpioexception.hpp>
#include <lib/drivers/include/thermoclick13.hpp>
#include <linux/i2c-dev.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

float ThermoClick::getTemperature() {
  int i2c_bus;
  uint8_t data[2];
  float temperature = -1;

  // Open i2c bus
  if ((i2c_bus = open(I2C_DEVICE, O_RDONLY)) < 0) {
    throw GpioException("Unable to open " I2C_DEVICE);
  }

  // Change slave to thermo sensor
  if (ioctl(i2c_bus, I2C_SLAVE, THERMO_13_ADDRESS) < 0) {
    throw GpioException(
        "Unable to select slave address " STR(THERMO_13_ADDRESS));
  }

  if (read(i2c_bus, &data, 2) != 2) {
    throw GpioException("Failed to read temperature sensor");
  }

  temperature = ((data[0] << 4) + (data[1] >> 4)) * CONVERSION_CONSTANT;

  return temperature;
}
