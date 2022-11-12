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

    def get_value(self) -> None:
        self.value = 1

    def get_state(self) -> dict:
        return {"unit_str": "apples"}


@pytest.fixture
def test_device():
    return Device("localhost", 8000, "testdevice", "1234", 5)


@pytest.fixture
def mock_sensor():
    return MockSensor("mocksensor", 3)


@pytest.fixture
def mock_sensor_2():
    return MockSensor("mocksensor", 3)


@pytest.fixture
def test_device_with_sensor(mock_sensor):
    dev = Device("localhost", 8000, "testdevice", "1234", 5)
    dev.sensor_a.append(mock_sensor)
    return dev


@pytest.fixture
def test_device_with_2_sensor(mock_sensor, mock_sensor_2):
    dev = Device("localhost", 8000, "testdevice", "1234", 5)
    dev.sensor_a.append(mock_sensor)
    dev.sensor_a.append(mock_sensor_2)
    return dev


@pytest.mark.parametrize(
    "test_case",
    open_json_file(
        "test/unit_test/device_tests/" + "device_post_sensor_readings.json"
    ),
)
def test_post_sensor_readings(test_case, request):
    dev = request.getfixturevalue(test_case["test_data"]["device"])
    with requests_mock.Mocker() as m:
        for sensor_i in dev.sensor_a:
            m.put(
                f"{dev.server_url}/api/devices/{dev.device_id}"
                + f"/sensors/{sensor_i.sensor_id}",
                status_code=test_case["test_data"]["status_code"],
            )

        if test_case["asserts"]["throw_connection_err"]:
            with pytest.raises(ConnectionError):
                dev.post_sensor_readings()
        else:
            dev.post_sensor_readings()

            history = m.request_history
        assert len(history) == len(dev.sensor_a)
        for hist_i in history:
            post_payload = hist_i.json()
            assert post_payload["apples"] == 1
