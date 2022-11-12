#pragma once
#include <include/actuator_interface.hpp>
#include <lib/drivers/include/driver_handler_interface.hpp>
#include <string>

class Actuator : public ActuatorInterface {
public:
  Actuator(const std::string &actuator_id,
           std::unique_ptr<DriverHandlerInterface> driver_handeler)
      : m_actuator_id{actuator_id}, m_triggered{false},
        m_driver_handeler{std::move(driver_handeler)} {};
  virtual ~Actuator() = default;
  virtual void trigger() = 0;
  virtual void resolve() = 0;

  const std::string &getActuatorId() const;

protected:
  const std::string m_actuator_id;
  bool m_triggered;
  std::unique_ptr<DriverHandlerInterface> m_driver_handeler;
};
