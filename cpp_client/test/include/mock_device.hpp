#include <include/device_interface.hpp>
#include <vector>

class MockDevice : public DeviceInterface {
public:
  MockDevice()
      : m_id("device"), m_json_o{web::json::value::object()},
        m_json_v{web::json::value::number(666)},
        m_frequency{100}, m_sensors{}, m_alarms{} {}

  bool serverIsReachable() override { return true; }

  void pingUntilReachableAndPostSensor() override {}

  void patchReportedProperties(const web::json::object &) override {}

  void addSensor(std::shared_ptr<SensorInterface> sensor) override {
    m_sensors.push_back(sensor);
  }

  void removeSensor(const std::string &sens_id) override {
    m_sensors.erase(
        std::remove_if(m_sensors.begin(), m_sensors.end(),
                       [&sens_id](std::shared_ptr<SensorInterface> const &a)
                           -> bool { return a->getId() == sens_id; }),
        m_sensors.end());
  }

  void postSensorReadings() override {}

  void setConnectionIndicator(bool) override {}

  void pollLoop() override { std::cout << "device is polling" << std::endl; }

  void triggerAlarm(AlarmInterface &) override {}

  void resolveAlarm(AlarmInterface &) override {}

  void triggerAndResolveAlarms() override {}

  /*std::vector<std::shared_ptr<AlarmInterface>> &listTriggeredAlarms() override
  { return m_alarms;
  }*/

  web::json::value getDesiredProperty(const std::string &) override {
    return m_json_v;
  }

  web::json::value fetchPropertiesFromServer() override { return m_json_o; }

  /*virtual std::shared_ptr<SensorInterface>
  getSensorAtIndex(int index) override {
    try {
      return m_sensors.at(index);
    } catch (std::out_of_range const &exc) {
      return nullptr;
    }
    return nullptr;
  };*/

  virtual int getFrequency() const override { return m_frequency; };

  virtual const std::string &getId() const override { return m_id; };

  void setFrequency(int newFrequency);

  void startReactor() override{};

private:
  const std::string m_id;
  web::json::value m_json_o;
  web::json::value m_json_v;
  int m_frequency;
  std::vector<std::shared_ptr<SensorInterface>> m_sensors;
  std::vector<std::shared_ptr<AlarmInterface>> m_alarms;
};

inline void MockDevice::setFrequency(int newFrequency) {
  m_frequency = newFrequency;
}
