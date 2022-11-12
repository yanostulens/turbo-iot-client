#include <test/include/mock_sensor.hpp>

MockSensor::MockSensor(const std::string &sensor_id, int frequency_ms)
    : m_sensor_id{sensor_id}, m_frequency_ms{frequency_ms} {}

void pollValue() {}

void MockSensor::addAlarm(std::shared_ptr<AlarmInterface> alarm) {
  m_alarms.push_back(std::move(alarm));
}

void MockSensor::removeAlarm(const std::string &alarm_id) {
  m_alarms.erase(
      std::remove_if(m_alarms.begin(), m_alarms.end(),
                     [&alarm_id](std::shared_ptr<AlarmInterface> const &a)
                         -> bool { return a.get()->getId() == alarm_id; }),
      m_alarms.end());
}

web::json::value MockSensor::getState() {
  web::json::value output;
  output["sensor_id"] = web::json::value(m_sensor_id);
  output["frequency_ms"] = web::json::value(m_frequency_ms);
  output["sensor_unit"] = web::json::value(m_unit);
  return output;
}

std::shared_ptr<SensorInterface>
SensorInterface::get(web::json::object const &obj) {
  std::string sensor_type = obj.at("type").as_string();
  std::shared_ptr<MockSensor> sensorFetch = nullptr;
  return sensorFetch;
}