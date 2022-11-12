import json
import pytest
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
        return {}


@pytest.fixture
def test_device():
    return Device(
        "10.32.10.59", 8000, "a_device", "BoTvBywIOAjhpTgoPldazUiv", 5
    )


@pytest.fixture
def mock_sensor():
    return MockSensor("mocksensor", 3)


@pytest.fixture
def test_device_with_sensor(mock_sensor):
    dev = Device(
        "10.32.10.59",
        8000,
        "a_device_with_sensor",
        "WlgGswlKZDXmOKVhqtDvThli",
        5,
    )
    dev.sensor_a.append(mock_sensor)
    return dev


@pytest.mark.parametrize(
    "test_case",
    open_json_file("test/integration_test/" + "device_add_sensor.json"),
)
def test_add_sensor(test_case, request):
    dev = request.getfixturevalue(test_case["test_data"]["device"])
    sensor = request.getfixturevalue(test_case["test_data"]["sensors"])

    assert len(dev.sensor_a) == test_case["asserts"]["init_sensor_len"]

    if test_case["asserts"]["throw_connection_err"]:
        with pytest.raises(ConnectionError):
            dev.add_sensor(sensor)
    else:
        dev.add_sensor(sensor)

    assert len(dev.sensor_a) == test_case["asserts"]["after_sensor_len"]


@pytest.mark.parametrize(
    "test_case",
    open_json_file("test/integration_test/" + "device_remove_sensor.json"),
)
def test_remove_sensor(test_case, request):
    dev = request.getfixturevalue(test_case["test_data"]["device"])

    assert len(dev.sensor_a) == test_case["asserts"]["init_sensor_len"]

    if test_case["asserts"]["throw_connection_err"]:
        with pytest.raises(ConnectionError):
            dev.remove_sensor("mocksensor")
    else:
        dev.remove_sensor("mocksensor")

    assert len(dev.sensor_a) == test_case["asserts"]["after_sensor_len"]
