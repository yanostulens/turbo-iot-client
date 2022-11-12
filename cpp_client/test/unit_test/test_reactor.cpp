#include "gmock/gmock.h"
#include <gtest/gtest.h>
#include <include/reactor.hpp>
#include <test/include/mock_device.hpp>
#include <test/include/mock_sensor.hpp>

class SensorFixture : public MockSensor {
public:
  MOCK_METHOD(void, pollValue, (), (override));

  SensorFixture(const std::string &id, int freq) : MockSensor(id, freq){};
  ~SensorFixture() { std::cout << this->getId() << " goes bye" << std::endl; };
};

class DeviceFixture : public MockDevice {
public:
  MOCK_METHOD(void, pollLoop, (), (override));
  DeviceFixture() : MockDevice() {}
};

class ReactorFixture : public ::testing::Test, public Reactor {
protected:
  std::shared_ptr<DeviceFixture> dev;
  std::shared_ptr<SensorFixture> sens1;
  std::shared_ptr<SensorFixture> sens2;
  virtual void SetUp() override {
    dev = std::make_shared<DeviceFixture>();
    sens1 = std::make_shared<SensorFixture>("sens1", 40);
    sens2 = std::make_shared<SensorFixture>("sens2", 70);
  }

  void printQueue() {
    auto &popped = m_timer_queue.top();
    std::visit(
        [popped](auto &&var) {
          if (auto p_dev_or_sens = var.lock()) {
            std::cout << p_dev_or_sens->getId() << " is next " << std::endl;
          }
        },
        popped.m_var);
  }
};

TEST_F(ReactorFixture, addToQueue) {
  this->addToQueue(dev);   // now +100 ms
  this->addToQueue(sens1); // now +40 ms
  this->addToQueue(sens2); // nog +70 ms
  ::testing::Sequence seq;

  EXPECT_CALL(*sens1, pollValue()).Times(1).InSequence(seq); // +40
  EXPECT_CALL(*sens2, pollValue()).Times(1).InSequence(seq); // +70
  EXPECT_CALL(*sens1, pollValue()).Times(1).InSequence(seq); // +80
  EXPECT_CALL(*dev, pollLoop()).Times(1).InSequence(seq);    // +100
  for (int i = 0; i < 4; i++) {
    this->pollLoop();
  }
}

TEST_F(ReactorFixture, weakPtrs) {
  dev->addSensor(sens1);
  dev->addSensor(sens2);
  this->addToQueue(dev);   // now + 100 ms
  this->addToQueue(sens1); // now + 40 ms
  this->addToQueue(sens2); // nog + 70 ms
  ::testing::Sequence seq;
  EXPECT_CALL(*sens1, pollValue()).Times(1).InSequence(seq); // +40
  this->pollLoop();
  EXPECT_CALL(*sens2, pollValue()).Times(1).InSequence(seq); // +70
  this->pollLoop();
  // remove sens1
  dev->removeSensor("sens1");
  sens1 = nullptr;
  // pop sens1 ptr, which should be next in queue @ +80 ms
  this->pollLoop();
  EXPECT_CALL(*dev, pollLoop()).Times(1).InSequence(seq); // +100
  this->pollLoop();
  EXPECT_CALL(*sens2, pollValue()).Times(1).InSequence(seq); // +140
  this->pollLoop();
  // remove sens2
  dev->removeSensor("sens2");
  sens2 = nullptr;
  EXPECT_CALL(*dev, pollLoop()).Times(1).InSequence(seq); // +200
  this->pollLoop();
  // pop sens2 ptr, which should be next in queue @ +210 ms
  this->pollLoop();
  EXPECT_CALL(*dev, pollLoop()).Times(1).InSequence(seq); // +300
  this->pollLoop();
}
