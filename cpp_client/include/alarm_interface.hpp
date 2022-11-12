#pragma once
#include <include/actuator_interface.hpp>
#include <include/alarmrule_interface.hpp>
#include <memory>
#include <string>

class AlarmInterface {
public:
  static std::shared_ptr<AlarmInterface> get(const web::json::object &obj);

  virtual ~AlarmInterface(){};
  virtual void triggerAlarm() = 0;
  virtual void resolveAlarm() = 0;
  virtual void addActuator(std::shared_ptr<ActuatorInterface>) = 0;
  virtual void triggerActuators() = 0;
  virtual const std::string &getId() const = 0;
  virtual const std::string &getDescr() const = 0;
  virtual const bool &isTriggered() const = 0;
  virtual std::shared_ptr<AlarmRuleInterface> getRule() = 0;
};
