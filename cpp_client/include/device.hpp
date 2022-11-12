#pragma once
#include <cpprest/http_client.h>
#include <include/config_interface.hpp>
#include <include/connection_exception.hpp>
#include <include/device_interface.hpp>
#include <include/http_handler_interface.hpp>
#include <include/reactor_interface.hpp>
#include <lib/clue.hpp>

class Device : public DeviceInterface,
               public std::enable_shared_from_this<Device> {
private:
  bool m_server_reachable_status;

protected:
  std::string m_device_id;
  int m_frequency_ms;
  std::shared_ptr<HttpHandlerInterface> m_http_handler;
  std::vector<std::shared_ptr<SensorInterface>> m_sensor_vector;
  std::shared_ptr<ReactorInterface> m_reactor;

public:
  Device(std::string const &device_id, int const frequency_ms,
         const std::string &server_url, const std::string &token,
         web::json::array const sensor_config_array);

  bool serverIsReachable() override;

  void pingUntilReachableAndPostSensor() override;

  void patchReportedProperties(const web::json::object &changes) override;

  void addSensor(std::shared_ptr<SensorInterface> sensor) override;

  void removeSensor(const std::string &sensor_id) override;

  void postSensorReadings() override;

  void setConnectionIndicator(bool state) override;

  void pollLoop() override;

  void triggerAlarm(AlarmInterface &alarm) override;

  void resolveAlarm(AlarmInterface &alarm) override;

  void triggerAndResolveAlarms() override;

  web::json::value listTriggeredAlarms() override;

  web::json::value getDesiredProperty(const std::string &key) override;

  web::json::value fetchPropertiesFromServer() override;

  // Getters
  const std::string &getId() const override;
  int getFrequency() const override;

  // std::vector<std::shared_ptr<SensorInterface>> &getSensors() override;

  // Setters
  void setFrequency(const int freq);

  void startReactor() override;
};
