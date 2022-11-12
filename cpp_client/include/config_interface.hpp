#pragma once
#include <cpprest/json.h>
#include <memory>

class ConfigInterface {
public:
  static std::shared_ptr<ConfigInterface> get(std::string const &config_file);
  virtual web::json::value query(std::string const &key) const = 0;
  virtual ~ConfigInterface(){};
};
