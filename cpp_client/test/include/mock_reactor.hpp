#pragma once
#include <cstdint>

#include <include/reactor_interface.hpp>

#include <queue>
#include <utility>
#include <variant>
#include <vector>

class MockReactor : public ReactorInterface {
public:
  void addToQueue(std::shared_ptr<DeviceInterface>) override{};
  void addToQueue(std::shared_ptr<SensorInterface>) override{};
  bool pollLoop() override { return true; };
};
