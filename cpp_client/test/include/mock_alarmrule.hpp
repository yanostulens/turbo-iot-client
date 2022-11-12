#pragma once
#include <include/alarmrule_interface.hpp>

class MockAlarmRule : public AlarmRuleInterface {
public:
  MockAlarmRule(float threshold, Condition condition);

  virtual bool checkCondition(float value) override;

  float getThreshold() const;
  void setThreshold(float newThreshold);

  Condition getCondition() const;
  void setCondition(Condition newCondition);

private:
  float m_threshold;
  Condition m_condition;
};
