import shutil
from turbo_client.sensors.sensor import Sensor


class StorageSensor(Sensor):
    unit_str: str

    def __init__(self, sensor_id: str, frequency: int):
        Sensor.__init__(self, sensor_id, frequency)
        self.get_value()
        self.unit_str = "GiB"

    def get_state(self) -> dict:
        return {
            "sensor_id": self.sensor_id,
            "frequency": self.frequency,
            self.unit_str: self.value,
            "unit_str": self.unit_str,
        }

    def get_value(self):
        result = shutil.disk_usage("/")
        self.value = result[0] // (2**30)
