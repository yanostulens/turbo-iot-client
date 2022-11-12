#pragma once
#include <lib/drivers/include/driver_handler.hpp>

#define I2C_DEVICE "/dev/i2c-2"
#define THERMO_13_ADDRESS 0x48
#define CONVERSION_CONSTANT 0.0625
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

class ThermoClick : public DriverHandler {
public:
  float getTemperature() override;
};
