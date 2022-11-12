#include <lib/drivers/include/driver_handler.hpp>

class Buzzer : public DriverHandler {
public:
  void setBuzzer(const int8_t value) override;
};
