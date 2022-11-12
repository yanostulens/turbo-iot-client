#include <include/config.hpp>
#include <include/file_exception.hpp>
#include <lib/clue.hpp>
#include <string>
Config::Config(std::string const &config_file) {
  std::ifstream input(config_file);
  if (!input) {
    throw FileException("Can't open file");
  }
  m_config = web::json::value::parse(input);
  if (!input) {
    throw FileException("Can't read file");
  }
  LOG_INFO("Loaded Config file");
}
web::json::value Config::query(std::string const &key) const {
  return m_config.at(U(key));
}
std::shared_ptr<ConfigInterface>
ConfigInterface::get(std::string const &config_file) {
  return std::make_shared<Config>(config_file);
}
