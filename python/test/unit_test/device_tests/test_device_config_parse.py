import json
from pathlib import Path

import pytest
import requests_mock
from turbo_client.actuators.buzzer_actuator import BuzzerActuator

from turbo_client.alarm import Alarm
from turbo_client.alarm_rule import AlarmRule, Condition
from turbo_client.device import Device
from turbo_client.sensors.sensor import Sensor
from turbo_client.sensors.storage_sensor import StorageSensor


def open_json_file(path: str):
    with open(path) as f:
        data = json.load(f)
    return data


def test_device_init_from_dict_blue_sky():
    data = open_json_file("test/unit_test/device_tests/config_blue_sky.json")
    dev = Device.init_from_dict(data)
    assert dev.device_id == "a_device"
    assert dev.token == "BoTvBywIOAjhpTgoPldazUiv"
    assert dev.frequency == 5
    assert dev.server_url == "http://10.32.10.59:8000"
    assert len(dev.sensor_a) == 1
    assert isinstance(dev.sensor_a[0], StorageSensor)
    assert dev.sensor_a[0].sensor_id == "storage-sens"
    assert dev.sensor_a[0].frequency == 3
    assert len(dev.sensor_a[0].alarm_a) == 1
    assert dev.sensor_a[0].alarm_a[0].alarm_id == "alarm"
    assert dev.sensor_a[0].alarm_a[0].description == "alarm descr"
    assert dev.sensor_a[0].alarm_a[0].rule.condition == Condition.EQUALS
    assert dev.sensor_a[0].alarm_a[0].rule.treshold == 1
    assert len(dev.sensor_a[0].alarm_a[0].actuator_a) == 1
    assert dev.sensor_a[0].alarm_a[0].actuator_a[0].actuator_id == "buzzer"
    assert isinstance(dev.sensor_a[0].alarm_a[0].actuator_a[0], BuzzerActuator)
