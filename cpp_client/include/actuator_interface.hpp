#pragma once
#include <cpprest/json.h>
#include <memory>

class ActuatorInterface {
public:
  static std::shared_ptr<ActuatorInterface> get(const web::json::object &obj);

  virtual ~ActuatorInterface(){};
  virtual void trigger() = 0;
  virtual void resolve() = 0;
};
