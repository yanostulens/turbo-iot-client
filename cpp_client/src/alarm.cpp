#include <include/alarm.hpp>
#include <include/alarmrule.hpp>

std::shared_ptr<AlarmInterface>
AlarmInterface::get(const web::json::object &obj) {
  auto rule = AlarmRuleInterface::get(obj.at(U("rule")).as_object());
  return std::make_shared<Alarm>(obj.at(U("alarm_id")).as_string(),
                                 obj.at(U("description")).as_string(),
                                 std::move(rule));
}

Alarm::Alarm(const std::string &id, const std::string &descr,
             std::shared_ptr<AlarmRuleInterface> rule)
    : m_id{id}, m_descr{descr}, m_actuators{},
      m_triggered{false}, m_rule{std::move(rule)} {}

void Alarm::triggerAlarm() { m_triggered = true; }

void Alarm::resolveAlarm() { m_triggered = false; }

void Alarm::addActuator(std::shared_ptr<ActuatorInterface> p_actuator) {
  m_actuators.push_back(std::move(p_actuator));
}

void Alarm::triggerActuators() {
  for (auto &actuator_p : m_actuators) {
    actuator_p->trigger();
  }
}

const std::string &Alarm::getId() const { return m_id; }

const std::string &Alarm::getDescr() const { return m_descr; }

const bool &Alarm::isTriggered() const { return m_triggered; }

const std::vector<std::shared_ptr<ActuatorInterface>> &Alarm::getActuators() {
  return m_actuators;
}

std::shared_ptr<AlarmRuleInterface> Alarm::getRule() { return m_rule; }
