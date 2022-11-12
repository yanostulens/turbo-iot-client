#pragma once
#include <cstdint>

#include <include/reactor_interface.hpp>

#include <queue>
#include <utility>
#include <variant>
#include <vector>

class Reactor : public ReactorInterface {
public:
  Reactor() : m_timer_queue{} {}
  void addToQueue(std::shared_ptr<DeviceInterface>) override;
  void addToQueue(std::shared_ptr<SensorInterface>) override;
  bool pollLoop() override;

protected:
  struct Task {
    std::chrono::steady_clock::time_point m_timestamp;
    std::variant<std::weak_ptr<DeviceInterface>, std::weak_ptr<SensorInterface>>
        m_var;
    Task(std::chrono::steady_clock::time_point const &timestamp,
         std::variant<std::weak_ptr<DeviceInterface>,
                      std::weak_ptr<SensorInterface>> const &var)
        : m_timestamp{timestamp}, m_var{var} {}

    bool operator<(const Task &t) const {
      return this->m_timestamp > t.m_timestamp;
    }
  };

  void poll(std::shared_ptr<DeviceInterface> dev);
  void poll(std::shared_ptr<SensorInterface> sens);

  std::chrono::steady_clock::time_point timeToSleep(int const ms);

  std::priority_queue<Task> m_timer_queue;
};
