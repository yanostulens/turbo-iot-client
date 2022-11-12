#pragma once
#include <cpprest/json.h>
#include <memory>

class AlarmRuleInterface {
public:
  static std::shared_ptr<AlarmRuleInterface> get(const web::json::object &obj);

  enum Condition { EQUALS, GREATER, SMALLER };
  virtual ~AlarmRuleInterface(){};
  virtual bool checkCondition(float value) = 0;
};
