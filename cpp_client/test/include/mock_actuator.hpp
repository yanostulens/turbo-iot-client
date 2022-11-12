#pragma once
#include <include/actuator_interface.hpp>

class MockActuator : public ActuatorInterface {
public:
  MockActuator(const std::string &actuator_id)
      : m_actuator_id(actuator_id), m_triggered(false){};
  virtual void trigger() override;
  virtual void resolve() override;
  bool getTriggered() const;

private:
  const std::string m_actuator_id;
  bool m_triggered;
};
