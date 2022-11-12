from turbo_client.sensors.hardware_sensors.hardware_sensor import (
    HardwareSensor,
)


class ThermoClickSensor(HardwareSensor):
    def __init__(self, sensor_id: str, frequency: int):
        HardwareSensor.__init__(self, sensor_id, frequency, "clicked")

    def get_command(self) -> str:
        return "nit_thermo_13_click"

    def parse_input(self, value: str) -> None:
        self.value = int(float(value))
