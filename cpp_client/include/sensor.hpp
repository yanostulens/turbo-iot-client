#pragma once

#include <cpprest/json.h>
#include <include/sensor_interface.hpp>
#include <lib/drivers/include/driver_handler_interface.hpp>
#include <string>
#include <vector>

class TempSensor;

class Sensor : public SensorInterface {

public:
  Sensor(std::string const &sensor_id, int frequency,
         std::unique_ptr<DriverHandlerInterface> driver_handeler,
         std::string unit);

  const std::string &getId() const override;

  int getFrequency() const override;
  float getValue() const override;

  void setFrequency(const int freq);
  void setValue(const float val);

  virtual web::json::value getState() override = 0;
  virtual void pollValue() override = 0;

  virtual void addAlarm(std::shared_ptr<AlarmInterface> alarm) override;

  virtual std::vector<std::shared_ptr<AlarmInterface>> &getAlarms() override;

  virtual void removeAlarm(const std::string &alarm_id) override;

  friend std::ostream &operator<<(std::ostream &os, const Sensor &s);

protected:
  std::string m_sensor_id;
  std::vector<std::unique_ptr<AlarmInterface>> m_alarms;

protected:
  int m_frequency_ms;
  std::unique_ptr<DriverHandlerInterface> m_driver_handeler;
  float m_value;
  std::string m_unit;
  std::vector<std::shared_ptr<AlarmInterface>> m_alarms;
};
