#include <test/include/mock_alarmrule.hpp>

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
  return std::make_shared<MockAlarmRule>(obj.at(U("threshold")).as_integer(),
                                         condition);
}

MockAlarmRule::MockAlarmRule(float threshold, Condition condition)
    : m_threshold{threshold}, m_condition{condition} {}

bool MockAlarmRule::checkCondition(float value) {
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

float MockAlarmRule::getThreshold() const { return m_threshold; }

void MockAlarmRule::setThreshold(float newThreshold) {
  m_threshold = newThreshold;
}

MockAlarmRule::Condition MockAlarmRule::getCondition() const {
  return m_condition;
}

void MockAlarmRule::setCondition(Condition newCondition) {
  m_condition = newCondition;
}
