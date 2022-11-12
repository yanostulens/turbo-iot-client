#include <include/temp_sensor.hpp>
#include <iostream>
#include <lib/drivers/include/thermoclick13.hpp>
TempSensor::TempSensor(std::string const &sensor_id, int frequency)
    : Sensor(sensor_id, frequency, std::make_unique<ThermoClick>()),
      m_unit{"°C"} {}

web::json::value TempSensor::getState() {
  web::json::value output;
  output["sensor_unit"] = web::json::value(m_unit);
  output["value"] = web::json::value(m_value);
  output["frequency_ms"] = web::json::value(m_frequency_ms);
  return output;
}
void TempSensor::pollValue() { m_value = m_driver_handeler->getTemperature(); }
