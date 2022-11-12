#include <test/include/mock_actuator.hpp>

std::shared_ptr<ActuatorInterface>
ActuatorInterface::get(const web::json::object &obj) {
  return std::make_shared<MockActuator>(obj.at(U("actuator_id")).as_string());
}

void MockActuator::trigger() { m_triggered = true; }

void MockActuator::resolve() { m_triggered = false; }

bool MockActuator::getTriggered() const { return m_triggered; }
