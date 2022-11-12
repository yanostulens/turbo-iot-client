#pragma once
#include <lib/drivers/include/driver_handler_interface.hpp>

class DriverHandler : public DriverHandlerInterface {
public:
  virtual void setBargraph(const std::string &value) override;
  virtual void setBuzzer(const int8_t value) override;

  virtual int get2x2Click(const int8_t button_index) override;
  virtual float getTemperature() override;
};
