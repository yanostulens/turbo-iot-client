from pathlib import Path

from turbo_client.config_parser import process_inputs
from turbo_client.device import Device
from turbo_client.logger import Logger
from turbo_client.sensors.sensor import Sensor

CONFIG_FILE = "config.json"
logger = Logger.get_logger()


class ExampleSensor(Sensor):
    def __init__(self, sensor_id: str, frequency: int):
        Sensor.__init__(self, sensor_id, frequency)

    def get_value(self) -> None:
        self.value = 1

    def get_state(self) -> dict:
        return {"unit_str": "°C"}

    def get_reading_title(self) -> str:
        return "°C"


def main():
    config_path = Path.cwd().joinpath(CONFIG_FILE)
    config = process_inputs(config_path)
    dev = Device.init_from_dict(config)
    print(config)
    dev.init_poll_loop()


if __name__ == "__main__":
    main()
