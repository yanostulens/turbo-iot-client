#include <include/buzzer_actuator.hpp>
#include <lib/drivers/include/buzzer.hpp>

BuzzerActuator::BuzzerActuator(const std::string &actuator_id)
    : Actuator(actuator_id, std::make_unique<Buzzer>()) {}

void BuzzerActuator::trigger() { m_driver_handeler->setBuzzer(1); }

void BuzzerActuator::resolve() { m_driver_handeler->setBuzzer(0); }
