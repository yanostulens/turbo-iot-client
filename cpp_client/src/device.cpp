#include <include/device.hpp>
#include <iostream>
#include <thread>

using HttpStatusCode = HttpHandlerInterface::HttpStatusCode;

/////////////////////! untested !//////////////////////

Device::Device(std::string const &device_id, int frequency_ms,
               const std::string &server_url, const std::string &token,
               web::json::array sensor_config_array)
    : m_server_reachable_status{false}, m_device_id{device_id},
      m_frequency_ms{frequency_ms},
      m_http_handler{std::move(HttpHandlerInterface::get(server_url, token))},
      m_sensor_vector{}, m_reactor{std::move(ReactorInterface::get())} {
  /**
   * Parameters:
   *  filename of the config file
   * Returns:
   *  Device
   **/
  LOG_DEBUG("Device Creation");
  for (web::json::value &sensor_config : sensor_config_array) {
    (*this).addSensor(SensorInterface::get(sensor_config.as_object()));
  }
};

const std::string &Device::getId() const { return m_device_id; }

int Device::getFrequency() const { return m_frequency_ms; }

void Device::setFrequency(int frequency_ms) { m_frequency_ms = frequency_ms; }

std::shared_ptr<DeviceInterface>
DeviceInterface::get(std::string const &filename) {
  LOG_DEBUG("Get Device Interface info");
  std::shared_ptr<ConfigInterface> config = ConfigInterface::get(filename);

  web::json::object server_config = config->query("server").as_object();
  std::string hostname = server_config.at("hostname").as_string();
  std::string port = server_config.at("port").as_string();
  std::string url = "http://" + hostname + ":" + port;

  web::json::object device_config = config->query("device").as_object();

  std::string device_id = device_config.at("device_id").as_string();
  std::string token = device_config.at("token").as_string();
  int frequency_ms = device_config.at("frequency").as_integer();

  web::json::array sensor_config_array = device_config.at("sensors").as_array();

  return std::make_shared<Device>(device_id, frequency_ms, url, token,
                                  sensor_config_array);
}

bool Device::serverIsReachable() {
  /*
   * Checks if the server is reachable by pinging it
   */
  web::http::http_response resp =
      (*m_http_handler).httpGet("/api/service/ping");
  if (resp.status_code() == HttpStatusCode::OK) {
    return true;
  } else {
    LOG_WARNING(std::string{"serverIsReachable: Error code " +
                            std::to_string(resp.status_code())});
    return false;
  }
}

void Device::pingUntilReachableAndPostSensor() {
  /*
   * Infinite loop until server is reachable
   */
  LOG_DEBUG("pingUntilReachableAndPostSensor");
  m_server_reachable_status = this->serverIsReachable();
  while (!m_server_reachable_status) {
    try {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_frequency_ms));
      m_server_reachable_status = this->serverIsReachable();
      for (auto &sens : m_sensor_vector) {
        this->addSensor(sens);
      }
    } catch (std::exception const &e) {
      LOG_WARNING("pingUntilReachable:: " + std::string{e.what()});
      m_server_reachable_status = false;

    } catch (...) {
      LOG_WARNING("pingUntilReachable:: unknown error caught");
      m_server_reachable_status = false;
    }
  }
}

void Device::patchReportedProperties(const web::json::object &changes) {
  /*
   * Parameters:
   *   json object containing the changes to patch with
   * Raises:
   *   ConnectionException on HTTP failure
   */
  LOG_DEBUG("patchReportedProperties");
  web::json::value properties = fetchPropertiesFromServer();
  web::json::object reported_properties_temp =
      properties.at(U("reported")).as_object();
  for (auto jt = changes.cbegin(); jt != changes.cend(); ++jt) {
    auto &key_changes = jt->first;
    auto &value_changes = jt->second;

    // I wonder if we still need a separate case for when the key doesn't exist
    // yet..
    bool property_exists = false;
    for (auto it = reported_properties_temp.cbegin();
         it != reported_properties_temp.cend(); ++it) {
      auto &key_properties = it->first;
      if (key_properties == key_changes) {
        property_exists = true;
        properties["reported"][key_properties] = value_changes;
        LOG_INFO("The reported " << key_properties << "has been changed to "
                                 << value_changes);
        break;
      }
    }
    if (!property_exists) {
      properties["reported"][key_changes] = value_changes;
      LOG_INFO("The reported " << key_changes << "has been added as "
                               << value_changes);
    }
  }

  web::http::http_response resp =
      (*m_http_handler)
          .httpPut(std::string{"/api/devices/" + m_device_id + "/properties"},
                   properties);
  if (resp.status_code() != HttpStatusCode::OK) {

    utility::stringstream_t stream;
    properties.serialize(stream);
    throw ConnectionException(
        std::string{"patchReportedProperties: Error code" +
                    std::to_string(resp.status_code()) + ": " +
                    utility::conversions::to_utf8string(resp.reason_phrase()) +
                    ", json object is " + stream.str()});
  }
}

void Device::addSensor(std::shared_ptr<SensorInterface> sensor) {
  /*
   * Raises:
   *   ConnectionException on HTTP failure
   */
  LOG_DEBUG("addSensor");
  std::string sensor_id = sensor->getId();

  auto it = find_if(m_sensor_vector.begin(), m_sensor_vector.end(),
                    [sensor_id](std::shared_ptr<SensorInterface> &p) {
                      return (p->getId() == sensor_id);
                    });
  if (it != std::end(m_sensor_vector)) {
    LOG_WARNING(std::string{"addSensor: sensor " + sensor_id +
                            " already present on device"});
    return;
  }

  web::json::value obj = web::json::value::parse(
      U(std::string{"{\"sensor_id\": \"" + sensor_id + "\"}"}));

  web::http::http_response resp =
      (*m_http_handler)
          .httpPost(std::string{"/api/devices/" + m_device_id + "/sensors"},
                    obj);
  if (resp.status_code() == HttpStatusCode::OK) {
    LOG_INFO(
        std::string{"addSensor: sensor " + sensor_id + " posted to server"});
    m_reactor->addToQueue(sensor);
    m_sensor_vector.push_back(std::move(sensor));
    return;
  }
  if (resp.status_code() == HttpStatusCode::NOT_FOUND) {
    LOG_WARNING(std::string{"addSensor: sensor " + sensor_id +
                            " was already registered"});
    m_reactor->addToQueue(sensor);
    m_sensor_vector.push_back(std::move(sensor));
    return;
  }

  utility::stringstream_t stream;
  obj.serialize(stream);
  throw ConnectionException(std::string{
      "addSensor: Error code:" + std::to_string(resp.status_code()) + ": " +
      utility::conversions::to_utf8string(resp.reason_phrase()) +
      ", json object is " + stream.str()});
}

void Device::removeSensor(const std::string &sensor_id) {
  /*
   * Parameters:
   *   id of the Sensor to remove
   * Raises:
   *   ConnectionException on HTTP failure
   */
  LOG_INFO(std::string{"removeSensor: removing sensor " + sensor_id});

  web::http::http_response resp =
      (*m_http_handler)
          .httpDelete(std::string{"/api/devices/" + m_device_id + "/sensors/" +
                                  sensor_id});

  if (resp.status_code() != HttpStatusCode::OK &&
      (resp.status_code() != HttpStatusCode::NOT_FOUND)) {
    throw ConnectionException(std::string{
        "removeSensor: Error code" + std::to_string(resp.status_code()) + ": " +
        utility::conversions::to_utf8string(resp.reason_phrase())});
  }
  if (resp.status_code() == HttpStatusCode::OK) {
    LOG_INFO(std::string{"removeSensor: sensor " + sensor_id + " removed"});
  } else {
    LOG_WARNING(
        std::string{"removeSensor: sensor " + sensor_id + " not registered"});
  }

  // this also works if Sensor was not inside m_sensor_vector in the first place
  auto it = std::remove_if(m_sensor_vector.begin(), m_sensor_vector.end(),
                           [sensor_id](std::shared_ptr<SensorInterface> &p) {
                             return (p->getId() == sensor_id);
                           });
  m_sensor_vector.erase(it, m_sensor_vector.end());
}

void Device::postSensorReadings() {
  /*
   * Loops over all the Device's Sensors and posts the measurements
   * Raises:
   *   ConnectionException on HTTP failure
   */
  LOG_DEBUG("postSensorReadings");
  for (std::shared_ptr<SensorInterface> &p : m_sensor_vector) {
    web::json::value p_state = p->getState();
    std::string unit = p_state.at("sensor_unit").as_string();
    std::string val = std::to_string(p->getValue());
    std::string sensor_id = p->getId();

    // std::cout << "sensor value: " << val << std::endl;
    // std::cout << "sensor unit: " << unit << std::endl;

    web::json::value obj = web::json::value::parse(
        U(std::string{"{\"" + unit + "\": \"" + val + "\"}"}));

    web::http::http_response resp =
        (*m_http_handler)
            .httpPut(std::string{"/api/devices/" + m_device_id + "/sensors/" +
                                 sensor_id},
                     obj);
    if (resp.status_code() != HttpStatusCode::OK) {
      utility::stringstream_t stream;
      obj.serialize(stream);
      throw ConnectionException(std::string{
          "postSensorReadings: Error code" +
          std::to_string(resp.status_code()) + ": " +
          utility::conversions::to_utf8string(resp.reason_phrase()) +
          ", json object is " + stream.str()});
    }
  }
}

void Device::triggerAlarm(AlarmInterface &alarm) {
  /*
   * Raises:
   *   ConnectionException on HTTP failure
   */
  std::string alarm_id = alarm.getId();
  LOG_DEBUG(std::string{"tiggerAlarm: trigger " + alarm_id});

  web::json::value alarm_data;
  alarm_data["alarm_id"] = web::json::value{alarm_id};
  alarm_data["description"] = web::json::value{alarm.getDescr()};

  web::http::http_response resp =
      (*m_http_handler)
          .httpPost(std::string{"/api/devices/" + m_device_id + "/alarms"},
                    alarm_data);

  if (resp.status_code() == HttpStatusCode::BAD_REQUEST) {
    LOG_WARNING(std::string{"Error code " + std::to_string(resp.status_code()) +
                            ": Alarm is already triggered"});
    alarm.triggerAlarm();
    return;
  } else if (resp.status_code() != HttpStatusCode::CREATED) {
    utility::stringstream_t stream;
    alarm_data.serialize(stream);
    throw ConnectionException(std::string{
        "triggerAlarm: Error code " + std::to_string(resp.status_code()) +
        ": " + resp.reason_phrase() + ", json object is " + stream.str()});
  }
  alarm.triggerAlarm();
}

void Device::resolveAlarm(AlarmInterface &alarm) {
  /*
   * Raises:
   *   ConnectionException on HTTP failure
   */
  LOG_INFO(std::string{"resolveAlarm: resolving alarm " + alarm.getId()});

  web::http::http_response resp =
      (*m_http_handler)
          .httpDelete(std::string{"/api/devices/" + m_device_id + "/alarms"});

  if (resp.status_code() == HttpStatusCode::BAD_REQUEST) {
    LOG_DEBUG(std::string{"resolveAlarm: Alarm " + alarm.getId() +
                          " is already registered"});
    alarm.resolveAlarm();
  } else if (resp.status_code() != HttpStatusCode::CREATED) {
    throw ConnectionException(std::string{"resolveAlarm: Error code " +
                                          std::to_string(resp.status_code()) +
                                          ": " + resp.reason_phrase()});
  }
  alarm.resolveAlarm();
}

void Device::triggerAndResolveAlarms() {
  LOG_DEBUG("triggerAndResolveAlarms");
  /**
   * Loops over the device's sensors, fetches the sensor value and checks if the
   *alarm is currently raised based on if the alarm SHOULD be raised according
   *to the sensor value, resolve or trigger Alarm may be called
   **/
  for (std::shared_ptr<SensorInterface> &ps : m_sensor_vector) {
    float val = ps->getValue();

    // std::cout << "float value: " << val << std::endl;

    for (std::shared_ptr<AlarmInterface> &pa : ps->getAlarms()) {
      bool triggered = pa->isTriggered();
      bool should_be_trigggered = pa->getRule()->checkCondition(val);
      if (!triggered && should_be_trigggered) {
        this->triggerAlarm((*pa)); // I think this will cause issues with shared
                                   // pointer and references..
      }
      if (triggered && !should_be_trigggered) {
        this->resolveAlarm((*pa));
      }
    }
  }
}

web::json::value Device::listTriggeredAlarms() {
  /*
   * Raises:
   *   ConnectionException on HTTP failure
   */
  LOG_DEBUG(std::string{"listTriggeredAlarms"});

  web::http::http_response resp =
      (*m_http_handler)
          .httpGet(std::string{"/api/devices/" + m_device_id + "/alarms"});

  if (resp.status_code() != HttpStatusCode::CREATED) {
    throw ConnectionException(std::string{"listTriggeredAlarms: Error code " +
                                          std::to_string(resp.status_code()) +
                                          ": " + resp.reason_phrase()});
  }
  auto result = resp.extract_json();
  return result.wait(); // ?????
}

web::json::value Device::getDesiredProperty(const std::string &key) {
  /*
   * Parameters:
   *   key of the property to be fetched, only desired properties
   * Returns:
   *   value with respect to the desired key
   */

  // should check if key exists and throw error if it doesn't...
  auto data = fetchPropertiesFromServer();
  return data["desired"][key];
}

web::json::value Device::fetchPropertiesFromServer() {
  /*
   * Returns:
   *   Dict with all properties (desired and reported)
   * Raises:
   *   Connection error when it cannot reach server
   */

  LOG_DEBUG(std::string{"fetchPropertiesFromServer"});

  web::http::http_response resp =
      (*m_http_handler)
          .httpGet(std::string{"/api/devices/" + m_device_id + "/properties"});

  if (resp.status_code() == HttpStatusCode::OK) {
    auto result = resp.extract_json();
    return result.wait();
  } else {
    throw ConnectionException(std::string{
        "fetchPropertiesFromServer: Error code " +
        std::to_string(resp.status_code()) + ": " + resp.reason_phrase()});
  }
}

void Device::setConnectionIndicator(bool) {
  /*
   * Description..
   * Raises:
   *   ...
   * Parameters:
   *   ...
   */
  // Yano does this...
}

void Device::pollLoop() {
  /*
   * Description: does all polling tasks for the device
   */
  /*
  try {
  */
  if (!m_server_reachable_status) {
    m_server_reachable_status = serverIsReachable();
  }
  if (m_server_reachable_status) {
    this->postSensorReadings();
    this->triggerAndResolveAlarms();
    // throw std::exception(); // stop execution to see sensor value
  }
  /*
  } catch (std::exception const &e) {
    LOG_WARNING("pollLoop:: " + std::string{e.what()});
  } catch (...) {
    LOG_WARNING("pollLoop:: unknown error caught");
  }
  */
}

void Device::startReactor() {
  m_reactor->addToQueue(shared_from_this());
  while (m_reactor->pollLoop()) {
  }
}
