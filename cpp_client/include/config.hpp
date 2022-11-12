#pragma once
#include <include/config_interface.hpp>

class Config : public ConfigInterface {
public:
  Config(std::string const &config_file);
  virtual web::json::value query(std::string const &key) const override;

private:
  web::json::value m_config;
};
