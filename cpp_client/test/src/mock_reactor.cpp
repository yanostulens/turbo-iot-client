#include <test/include/mock_reactor.hpp>
std::shared_ptr<ReactorInterface> ReactorInterface::get() {
  return std::make_shared<MockReactor>();
}
