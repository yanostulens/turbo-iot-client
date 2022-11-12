#include <test/include/mock_config.hpp>
std::shared_ptr<ConfigInterface> ConfigInterface::get(std::string const &) {
  return std::make_shared<MockConfig>();
}
