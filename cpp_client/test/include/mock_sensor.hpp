#pragma once
#include <include/sensor_interface.hpp>

#include <string>
#include <vector>

class MockSensor : public SensorInterface {
private:
  const std::string m_sensor_id;
  const int m_frequency_ms;
  const std::string m_unit;
  std::vector<std::shared_ptr<AlarmInterface>> m_alarms;

public:
  MockSensor(const std::string &sensor_id, int frequency_ms);
  virtual int getFrequency() const override { return m_frequency_ms; };
  virtual const std::string &getId() const override { return m_sensor_id; };
  virtual web::json::value getState() override;
  virtual void pollValue() override{};
  virtual void addAlarm(std::shared_ptr<AlarmInterface> alarm) override;
  virtual void removeAlarm(const std::string &alarm_id) override;
  virtual std::vector<std::shared_ptr<AlarmInterface>> &getAlarms() override {
    return m_alarms;
  };
};
