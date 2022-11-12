#pragma once
#include <include/alarm_interface.hpp>
#include <include/alarmrule_interface.hpp>

#include <string>
#include <vector>

class Alarm : public AlarmInterface {
public:
  Alarm(const std::string &id, const std::string &descr,
        std::shared_ptr<AlarmRuleInterface> rule);

  virtual void triggerAlarm() override;

  virtual void resolveAlarm() override;

  virtual void
  addActuator(std::shared_ptr<ActuatorInterface> p_actuator) override;

  virtual void triggerActuators() override;

  virtual const std::string &getId() const override;

  virtual const std::string &getDescr() const override;

  const bool &isTriggered() const override;

  const std::vector<std::shared_ptr<ActuatorInterface>> &getActuators();

  std::shared_ptr<AlarmRuleInterface> getRule() override;

protected:
  const std::string m_id;
  const std::string m_descr;
  std::vector<std::shared_ptr<ActuatorInterface>> m_actuators;
  bool m_triggered;
  std::shared_ptr<AlarmRuleInterface> m_rule;
};
