#pragma once

#include <filesystem>
#include <string>
#include <sys/statvfs.h>

#include <include/sensor.hpp>

namespace fs = std::filesystem;

class StorageSensor : public Sensor {

private:
  fs::space_info tmp = fs::space("/tmp");

public:
  StorageSensor(std::string const &sensor_id, int frequency);
  web::json::value getState() override;
  void pollValue() override { m_value = tmp.free; };
};