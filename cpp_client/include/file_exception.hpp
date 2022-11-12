#include <include/turbo_exception.hpp>

class FileException : public TurboException {
public:
  explicit FileException(std::string const &issue) : TurboException(issue) {}
};
