#include <include/click_sensor.hpp>
#include <iostream>
#include <lib/drivers/include/keyclick2x2.hpp>
ClickSensor::ClickSensor(std::string const &sensor_id, int frequency, int index)
    : Sensor(sensor_id, frequency, std::make_unique<KeyClick2x2>()),
      m_unit{"clicks"}, m_index{index} {}

web::json::value ClickSensor::getState() {
  web::json::value output;
  output["sensor_unit"] = web::json::value(m_unit);
  output["value"] = web::json::value(m_value);
  output["frequency_ms"] = web::json::value(m_frequency_ms);
  output["index"] = web::json::value(m_index);
  return output;
}
void ClickSensor::pollValue() {
  m_value = m_driver_handeler->get2x2Click(m_index);
}
