import json

import pytest
import requests_mock

from turbo_client.device import Device
from turbo_client.sensors.sensor import Sensor


def open_json_file(path: str):
    with open(path) as f:
        data = json.load(f)
    return data


class MockSensor(Sensor):
    def __init__(self, sensor_id: str, frequency: int):
        Sensor.__init__(self, sensor_id, frequency)

    def get_value(self) -> int:
        return 1

    def get_state(self) -> dict:
        return {}


@pytest.fixture
def test_device():
    return Device("localhost", 8000, "testdevice", "1234", 5)


@pytest.fixture
def mock_sensor():
    return MockSensor("mocksensor", 3)


@pytest.fixture
def test_device_with_sensor(mock_sensor):
    dev = Device("localhost", 8000, "testdevice", "1234", 5)
    dev.sensor_a.append(mock_sensor)
    return dev


def test_get_headers(test_device):
    header = test_device.get_headers()
    assert header["Authorization"] == "Bearer 1234"


@pytest.mark.parametrize(
    "test_case",
    open_json_file("test/unit_test/device_tests/" + "device_add_sensor.json"),
)
def test_add_sensor(test_case, request):
    dev = request.getfixturevalue(test_case["test_data"]["device"])
    sensor = request.getfixturevalue(test_case["test_data"]["sensors"])
    with requests_mock.Mocker() as m:

        assert len(dev.sensor_a) == test_case["asserts"]["init_sensor_len"]

        m.post(
            f"{dev.server_url}/api/devices/{dev.device_id}/sensors",
            status_code=test_case["test_data"]["status_code"],
            json=test_case["test_data"]["json"],
        )

        if test_case["asserts"]["throw_connection_err"]:
            with pytest.raises(ConnectionError):
                dev.add_sensor(sensor)
        else:
            dev.add_sensor(sensor)

        assert len(dev.sensor_a) == test_case["asserts"]["after_sensor_len"]


@pytest.mark.parametrize(
    "test_case",
    open_json_file(
        "test/unit_test/device_tests/" + "device_remove_sensor.json"
    ),
)
def test_remove_sensor(test_case, request):
    dev = request.getfixturevalue(test_case["test_data"]["device"])
    sensor = request.getfixturevalue(test_case["test_data"]["sensors"])
    with requests_mock.Mocker() as m:

        assert len(dev.sensor_a) == test_case["asserts"]["init_sensor_len"]

        m.delete(
            f"{dev.server_url}/api/devices/{dev.device_id}/sensors/{sensor.sensor_id}",
            status_code=test_case["test_data"]["status_code"],
            json=test_case["test_data"]["json"],
        )

        if test_case["asserts"]["throw_connection_err"]:
            with pytest.raises(ConnectionError):
                dev.remove_sensor("mocksensor")
        else:
            dev.remove_sensor("mocksensor")

        assert len(dev.sensor_a) == test_case["asserts"]["after_sensor_len"]
