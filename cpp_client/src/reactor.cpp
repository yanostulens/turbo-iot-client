#include <include/reactor.hpp>
#include <lib/clue.hpp>
#include <thread>

std::shared_ptr<ReactorInterface> ReactorInterface::get() {
  return std::make_shared<Reactor>();
}

std::chrono::steady_clock::time_point Reactor::timeToSleep(int ms) {
  std::chrono::steady_clock::time_point x = std::chrono::steady_clock::now();
  x += std::chrono::milliseconds(ms);
  return x;
}

void Reactor::addToQueue(std::shared_ptr<DeviceInterface> dev) {

  m_timer_queue.emplace(timeToSleep(dev->getFrequency()),
                        std::variant<std::weak_ptr<DeviceInterface>,
                                     std::weak_ptr<SensorInterface>>(dev));
}

void Reactor::addToQueue(std::shared_ptr<SensorInterface> sens) {
  m_timer_queue.emplace(timeToSleep(sens->getFrequency()),
                        std::variant<std::weak_ptr<DeviceInterface>,
                                     std::weak_ptr<SensorInterface>>(sens));
}

bool Reactor::pollLoop() {
  std::this_thread::sleep_until(m_timer_queue.top().m_timestamp);
  auto popped = m_timer_queue.top();
  m_timer_queue.pop();

  std::visit(
      [&popped, this](auto &&var) {
        if (auto p_dev_or_sens = var.lock()) {
          this->poll(p_dev_or_sens);
          popped.m_timestamp = timeToSleep(p_dev_or_sens->getFrequency());
          m_timer_queue.push(popped);
        } else {
          LOG_INFO("A pointer expired");
        }
      },
      popped.m_var);
  return !m_timer_queue.empty();
}

void Reactor::poll(std::shared_ptr<DeviceInterface> dev) { dev->pollLoop(); }

void Reactor::poll(std::shared_ptr<SensorInterface> sens) { sens->pollValue(); }
