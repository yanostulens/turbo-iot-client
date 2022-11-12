#pragma once

#include <cpprest/json.h>
#include <include/alarm_interface.hpp>
#include <memory>
#include <string>

class SensorInterface {
public:
  virtual ~SensorInterface(){};

  virtual void pollValue() = 0;

  virtual void addAlarm(std::shared_ptr<AlarmInterface> alarm) = 0;

  virtual void removeAlarm(const std::string &alarm_id) = 0;

  virtual int getFrequency() const = 0;

  static std::shared_ptr<SensorInterface> get(web::json::object const &obj);

  virtual web::json::value getState() = 0;
  virtual const std::string &getId() const = 0;
  virtual std::vector<std::shared_ptr<AlarmInterface>> &getAlarms() = 0;

  virtual float getValue() const = 0;
};
