import json

import pytest
import requests_mock
from turbo_client.alarm_rule import AlarmRule, Condition

from turbo_client.device import Device
from turbo_client.sensors.sensor import Sensor


def open_json_file(path: str):
    with open(path) as f:
        data = json.load(f)
    return data


class MockSensor(Sensor):
    def __init__(self, sensor_id: str, frequency: int):
        Sensor.__init__(self, sensor_id, frequency)

    def get_value(self) -> None:
        self.value = 1

    def get_state(self) -> dict:
        return {"unit_str": "apples"}


@pytest.fixture
def mock_sensor():
    sens = MockSensor("mocksensor", 3)
    # this rule always triggers
    sens.define_alarm(
        "alarm1",
        "equals",
        AlarmRule(condition=Condition.EQUALS, treshold=1),
    )
    # this rull will do nothing, not triggerd and not satisfying treshold
    sens.define_alarm(
        "alarm2",
        "smmaller_than",
        AlarmRule(condition=Condition.SMALLER_THAN, treshold=1),
    )
    sens.alarm_a[1].triggered = True
    return sens


@pytest.fixture
def mock_sensor_2():
    sens = MockSensor("mocksensor", 3)
    # this rule always gets resolved
    sens.define_alarm(
        "alarm3",
        "greater_than",
        AlarmRule(condition=Condition.GREATER_THAN, treshold=1),
    )
    return sens


@pytest.fixture
def test_device_with_2_sensor(mock_sensor, mock_sensor_2):
    dev = Device("localhost", 8000, "testdevice", "1234", 5)
    dev.sensor_a.append(mock_sensor)
    dev.sensor_a.append(mock_sensor_2)
    return dev


def test_post_sensor_readings(test_device_with_2_sensor):
    dev = test_device_with_2_sensor
    with requests_mock.Mocker() as m:

        for sensor_i in dev.sensor_a:
            for alarm_i in sensor_i.alarm_a:
                # mock every possible delete
                request_path = (
                    f"/api/devices/{dev.device_id}"
                    + f"/alarms/{alarm_i.alarm_id}"
                )
                request_url = dev.server_url + request_path
                status_code = 200
                m.delete(request_url, status_code=status_code)
                # mock every possible put
                request_path = f"/api/devices/{dev.device_id}/alarms"
                request_url = dev.server_url + request_path
                status_code = 201
                text = [
                    {"alarm_id": "alarm1", "description": "des"},
                    {"alarm_id": "alarm2", "description": "des2"},
                ]
                m.post(request_url, json=text, status_code=status_code)

        dev.trigger_and_resolve_alarms()

        history = m.request_history
        assert len(history) == 2  # one trigger and one resolve
        assert history[0].method == "POST"
        post_payload = history[0].json()
        assert post_payload["alarm_id"] == "alarm1"
        assert post_payload["description"] == "equals"
        assert history[1].method == "DELETE"
