#include <fstream>
#include <lib/clue.hpp>
class TurboException : public std::exception {
public:
  explicit TurboException(std::string const &issue) : m_issue{issue} {
    LOG_ERROR(issue);
  }
  virtual const char *what() const throw() { return m_issue.c_str(); }

private:
  std::string const m_issue;
};
