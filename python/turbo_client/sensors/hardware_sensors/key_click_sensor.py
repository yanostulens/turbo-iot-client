from turbo_client.sensors.hardware_sensors.hardware_sensor import (
    HardwareSensor,
)


class KeyClickSensor(HardwareSensor):
    index: int

    def __init__(self, sensor_id: str, frequency: int, index: int):
        if index > 3 or index < 0:
            raise ValueError("Please provide a number between 0 and 3")
        self.index = index
        HardwareSensor.__init__(self, sensor_id, frequency, "clicked")

    def get_command(self) -> str:
        return "nit_2x2_key_click"

    def parse_input(self, value: str) -> None:
        value_list = value.split()
        self.value = int(value_list[self.index])
