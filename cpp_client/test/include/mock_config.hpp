#pragma once
#include <include/config_interface.hpp>

class MockConfig : public ConfigInterface {
public:
  virtual web::json::value query(std::string const &) const override {
    return web::json::value::number(666);
  }

private:
  web::json::value m_config;
};
