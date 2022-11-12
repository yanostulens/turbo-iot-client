from abc import abstractmethod
from turbo_client.sensors.sensor import Sensor
import logging
import subprocess

logger = logging.getLogger("main")


class HardwareSensor(Sensor):
    unit_str: str

    def __init__(self, sensor_id: str, frequency: int, unit: str):
        Sensor.__init__(self, sensor_id, frequency)
        self.unit_str = unit
        self.get_value()

    def get_state(self) -> dict:
        return {
            "sensor_id": self.sensor_id,
            "frequency": self.frequency,
            self.unit_str: self.value,
            "unit_str": self.unit_str,
        }

    def get_value(self):
        args = self.get_command()
        try:
            popen = subprocess.Popen(args, stdout=subprocess.PIPE)
            popen.wait()
            self.parse_input(popen.stdout.read())
        except FileNotFoundError:
            logger.warning(
                "trigger_command: subprocess failed, %s",
                "you're probably not on the beagle",
            )
            self.value = 0

    @abstractmethod
    def get_command(self) -> str:
        pass

    @abstractmethod
    def parse_input(self, value: str) -> None:
        pass
