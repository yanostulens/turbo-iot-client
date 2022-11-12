#include <test/include/mock_alarm.hpp>
#include <test/include/mock_alarmrule.hpp>

std::shared_ptr<AlarmInterface> AlarmInterface::get(const web::json::object &) {
  return std::make_shared<MockAlarm>("hello", "hello");
}

MockAlarm::MockAlarm(const std::string &id, const std::string &descr)
    : m_id{id}, m_descr{descr}, m_actuators{}, m_triggered{false} {}

void MockAlarm::triggerAlarm() { m_triggered = true; }

void MockAlarm::resolveAlarm() { m_triggered = false; }

void MockAlarm::addActuator(std::shared_ptr<ActuatorInterface> p_actuator) {
  m_actuators.push_back(std::move(p_actuator));
}

void MockAlarm::triggerActuators() {
  for (auto &actuator_p : m_actuators) {
    actuator_p->trigger();
  }
}

const std::string &MockAlarm::getId() const { return m_id; }

const std::string &MockAlarm::getDescr() const { return m_descr; }

const bool &MockAlarm::isTriggered() const { return m_triggered; }

const std::vector<std::shared_ptr<ActuatorInterface>> &
MockAlarm::getActuators() {
  return m_actuators;
}
