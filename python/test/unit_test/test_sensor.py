import mock

from turbo_client.sensors.cpu_temp_sensor import CpuTemperatureSensor
from turbo_client.sensors.storage_sensor import StorageSensor


def new_get_value_storage(*args, **kwargs):
    return 42


def new_get_value_cpu(*args, **kwargs):
    return 42


def test_StorageSensor():
    test_sensor = StorageSensor("storage_id", 1)
    with mock.patch.object(test_sensor, "value", new_get_value_storage()):
        assert test_sensor.get_state() == {
            "sensor_id": "storage_id",
            "frequency": 1,
            "GiB": new_get_value_storage(),
            "unit_str": "GiB",
        }


def test_TemperatureSensor():
    test_sensor = CpuTemperatureSensor("cpu_id", 1)
    with mock.patch.object(test_sensor, "value", new_get_value_cpu()):
        assert test_sensor.get_state() == {
            "sensor_id": "cpu_id",
            "frequency": 1,
            "°C": new_get_value_cpu(),
            "unit_str": "°C",
        }
