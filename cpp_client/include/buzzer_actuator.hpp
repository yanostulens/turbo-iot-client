#pragma once

#include <include/actuator.hpp>
#include <include/sensor.hpp>
#include <string>

class BuzzerActuator : public Actuator {
public:
  BuzzerActuator(std::string const &actuator_id);
  virtual void trigger() override;
  virtual void resolve() override;
};
