#include <include/actuator.hpp>
#include <iostream>
const std::string &Actuator::getActuatorId() const { return m_actuator_id; }

std::shared_ptr<ActuatorInterface>
ActuatorInterface::get(const web::json::object &obj) {
  // TODO
  std::cout
      << "there are no concrete actuators yet you wanted to create actuator "
      << obj.at(U("actuator_id")).as_string() << " which is of type "
      << obj.at(U("type")).as_string() << std::endl;
  return nullptr;
}
