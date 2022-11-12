#pragma once

#include <include/sensor.hpp>
#include <string>

class ClickSensor : public Sensor {
public:
  ClickSensor(std::string const &sensor_id, int frequency, int index);
  web::json::value getState() override;
  void pollValue() override;

private:
  const std::string m_unit;
  const int m_index;
};
