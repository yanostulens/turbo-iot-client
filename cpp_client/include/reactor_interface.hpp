#pragma once
#include <include/device_interface.hpp>
#include <include/reactor_interface.hpp>
#include <include/sensor_interface.hpp>
#include <memory>
#include <string>

class ReactorInterface {
public:
  virtual ~ReactorInterface() = default;
  virtual void addToQueue(std::shared_ptr<DeviceInterface>) = 0;
  virtual void addToQueue(std::shared_ptr<SensorInterface>) = 0;
  virtual bool pollLoop() = 0;

  static std::shared_ptr<ReactorInterface> get();
};
