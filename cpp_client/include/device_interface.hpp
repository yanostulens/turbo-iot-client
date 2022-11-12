#pragma once
#include <cpprest/json.h>
#include <include/alarm_interface.hpp>
#include <include/sensor_interface.hpp>
#include <memory>
#include <vector>

class DeviceInterface {
public:
  virtual ~DeviceInterface() {}

  virtual bool serverIsReachable() = 0;

  virtual void pingUntilReachableAndPostSensor() = 0;

  virtual web::json::value fetchPropertiesFromServer() = 0;

  virtual web::json::value getDesiredProperty(const std::string &key) = 0;

  virtual void patchReportedProperties(const web::json::object &changes) = 0;

  virtual void addSensor(std::shared_ptr<SensorInterface> sensor) = 0;

  virtual void removeSensor(const std::string &sensor_id) = 0;

  virtual void triggerAlarm(AlarmInterface &alarm) = 0;

  virtual web::json::value listTriggeredAlarms() = 0;

  virtual void resolveAlarm(AlarmInterface &alarm) = 0;

  virtual void triggerAndResolveAlarms() = 0;

  virtual void postSensorReadings() = 0;

  virtual void setConnectionIndicator(bool state) = 0;

  virtual void pollLoop() = 0;

  static std::shared_ptr<DeviceInterface> get(std::string const &filename);

  virtual const std::string &getId() const = 0;
  virtual int getFrequency() const = 0;

  virtual void startReactor() = 0;
};
