#include <include/turbo_exception.hpp>

class ConnectionException : public TurboException {
public:
  explicit ConnectionException(std::string const &issue)
      : TurboException(issue) {}
};
