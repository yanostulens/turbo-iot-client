#include <include/device.hpp>
#include <lib/clue.hpp>

int main(int argc, char const *argv[]) {
  if (argc == 1) {
    LOG_INFO("Amazing C++ application " << argv[0] << " started");
  }
  // init
  auto device = DeviceInterface::get("config.json");

  // manually call first device poll
  device->pollLoop();
  // start polling with reactor
  device->startReactor();
  return 0;
}
