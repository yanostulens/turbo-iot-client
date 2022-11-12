#include <include/bargraph_actuator.hpp>
#include <lib/drivers/include/bargraph.hpp>

BargraphActuator::BargraphActuator(const std::string &actuator_id)
    : Actuator(actuator_id, std::make_unique<Bargraph>()) {}

void BargraphActuator::trigger() {
  m_driver_handeler->setBargraph("0x1111111111");
}

void BargraphActuator::resolve() {
  m_driver_handeler->setBargraph("0x2222222222");
}
