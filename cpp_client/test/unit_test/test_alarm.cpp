#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <include/alarm.hpp>
#include <test/include/mock_actuator.hpp>
#include <test/include/mock_alarmrule.hpp>

class ActuatorFixture : public MockActuator {
public:
  MOCK_METHOD(void, trigger, (), (override));

  ActuatorFixture(const std::string &id) : MockActuator(id){};
};

class AlarmFixture : public ::testing::Test {
protected:
  std::shared_ptr<Alarm> alarm_handler;
  std::shared_ptr<ActuatorInterface> actuator_handler;
  web::json::value obj_alarm;
  web::json::value obj_alarm_rule;
  web::json::value obj_actuator;
  virtual void SetUp() {
    obj_alarm_rule["condition"] = web::json::value::string(">");
    obj_alarm_rule["threshold"] = web::json::value::number(28);

    obj_actuator["actuator_id"] = web::json::value::string("r_actuator_id");
    obj_actuator["type"] = web::json::value::string("r_type");
    this->actuator_handler = std::make_shared<ActuatorFixture>("r_actuator_id");

    obj_alarm["alarm_id"] = web::json::value::string("r_alarm_id");
    obj_alarm["description"] = web::json::value::string("r_description");
    obj_alarm["rule"] = obj_alarm_rule;
    this->alarm_handler = std::make_shared<Alarm>(
        "r_alarm_id", "r_description",
        std::move(AlarmRuleInterface::get(obj_alarm_rule.as_object())));
  }
};

TEST_F(AlarmFixture, GET_ALARM_ID) {
  auto response = alarm_handler->getId();
  EXPECT_EQ(response, "r_alarm_id");
}

TEST_F(AlarmFixture, GET_ALARM_DESCRIPTION) {
  auto response = alarm_handler->getDescr();
  EXPECT_EQ(response, "r_description");
}

TEST_F(AlarmFixture, GET_ALARM_TRIGG) {
  auto response = alarm_handler->isTriggered();
  EXPECT_EQ(response, false);
}

TEST_F(AlarmFixture, TRIGG_ALARM) {
  alarm_handler->triggerAlarm();
  auto response = alarm_handler->isTriggered();
  EXPECT_EQ(response, true);
}

TEST_F(AlarmFixture, RESOLVE_ALARM) {
  alarm_handler->triggerAlarm();
  auto response = alarm_handler->isTriggered();
  EXPECT_EQ(response, true);
  alarm_handler->resolveAlarm();
  response = alarm_handler->isTriggered();
  EXPECT_EQ(response, false);
}

TEST_F(AlarmFixture, ADD_ACTUATOR) {
  alarm_handler->addActuator(std::move(actuator_handler));
  EXPECT_EQ(alarm_handler->getActuators().size(), 1);
}
