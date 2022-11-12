#include <include/storage_sensor.hpp>
#include <iostream>

StorageSensor::StorageSensor(std::string const &sensor_id, int frequency)
    : Sensor(sensor_id, frequency, "%") {}

web::json::value StorageSensor::getState() {
  web::json::value output;

  output["sensor_unit"] = web::json::value(m_unit);
  output["value"] = web::json::value(tmp.free);
  output["frequency_ms"] = web::json::value(m_frequency_ms);
  return output;
}