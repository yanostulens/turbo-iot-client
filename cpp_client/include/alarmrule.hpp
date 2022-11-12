#pragma once
#include <include/alarmrule_interface.hpp>

class AlarmRule : public AlarmRuleInterface {
public:
  AlarmRule(float threshold, Condition condition);

  virtual bool checkCondition(float value) override;

  float getThreshold() const;
  void setThreshold(float newThreshold);

  Condition getCondition() const;

private:
  float m_threshold;
  const Condition m_condition;
};
