#pragma once

#include <include/sensor.hpp>
#include <string>

class TempSensor : public Sensor {

public:
  TempSensor(std::string const &sensor_id, int frequency, std::string unit);
  web::json::value getState() override;
  void pollValue() override;
};
