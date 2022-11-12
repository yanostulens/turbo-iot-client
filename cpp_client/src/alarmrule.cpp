#include <include/alarmrule.hpp>

std::shared_ptr<AlarmRuleInterface>
AlarmRuleInterface::get(const web::json::object &obj) {
  Condition condition;
  std::string condition_str = obj.at(U("condition")).as_string();
  if (condition_str == ">") {
    condition = Condition::GREATER;
  } else if (condition_str == "<") {
    condition = Condition::SMALLER;
  } else {
    condition = Condition::EQUALS;
  }
  return std::make_shared<AlarmRule>(obj.at(U("threshold")).as_integer(),
                                     condition);
}

AlarmRule::AlarmRule(float threshold, Condition condition)
    : m_threshold{threshold}, m_condition{condition} {}

bool AlarmRule::checkCondition(float value) {
  switch (m_condition) {
  case Condition::EQUALS:
    return value == m_threshold;
  case Condition::SMALLER:
    return value < m_threshold;
  case Condition::GREATER:
    return value > m_threshold;
  }
  return false;
}

float AlarmRule::getThreshold() const { return m_threshold; }

void AlarmRule::setThreshold(float newThreshold) { m_threshold = newThreshold; }

AlarmRule::Condition AlarmRule::getCondition() const { return m_condition; }
