#include <include/sensor.hpp>
#include <include/temp_sensor.hpp>

Sensor::Sensor(std::string const &sensor_id, int frequency,
               std::unique_ptr<DriverHandlerInterface> driver_handeler,
               std::string unit)
    : m_sensor_id(sensor_id),

      m_frequency_ms(frequency), m_value{0.00f}, m_unit{unit}, m_alarms{},
      m_driver_handeler{std::move(driver_handeler)} {}

const std::string &Sensor::getId() const { return m_sensor_id; }

int Sensor::getFrequency() const { return m_frequency_ms; }

float Sensor::getValue() const { return m_value; }

void Sensor::setFrequency(const int freq) { m_frequency_ms = freq; }

void Sensor::setValue(float val) { m_value = val; }

void Sensor::addAlarm(std::shared_ptr<AlarmInterface> alarm) {
  m_alarms.push_back(std::move(alarm));
}

void Sensor::removeAlarm(const std::string &alarm_id) {
  m_alarms.erase(
      std::remove_if(m_alarms.begin(), m_alarms.end(),
                     [&alarm_id](std::shared_ptr<AlarmInterface> const &a)
                         -> bool { return a.get()->getId() == alarm_id; }),
      m_alarms.end());
}

std::vector<std::shared_ptr<AlarmInterface>> &Sensor::getAlarms() {
  return m_alarms;
}

std::ostream &operator<<(std::ostream &os, const Sensor &s) {
  os << "sensor id: " << s.getId() << "frequency: " << s.getFrequency()
     << "value: " << s.getValue();
  return os;
}

std::shared_ptr<SensorInterface>
SensorInterface::get(web::json::object const &obj) {
  std::string sensor_type = obj.at("type").as_string();
  std::shared_ptr<Sensor> sensorFetch = nullptr;

  if (sensor_type == "temp") {
    std::cout << "unit in Sensor get: " << obj.at("unit").as_string()
              << std::endl;

    sensorFetch = std::make_shared<TempSensor>(
        obj.at("sensor_id").as_string(), obj.at("frequency_ms").as_integer(),
        obj.at("unit").as_string());
  }
  // no clause yet for buttonsensor: remove buttonsensor from config to prevent
  // nullptr
  return sensorFetch;
}
