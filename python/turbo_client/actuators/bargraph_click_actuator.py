from turbo_client.actuators.actuator import Actuator


class BarGraphClickActuator(Actuator):
    value: int

    def __init__(self, actuator_id: str):
        Actuator.__init__(self, actuator_id)
        self.value = 0

    def get_command(self) -> str:
        return "nit_bargraph_click"

    def trigger(self) -> None:
        """Toggles between red and blue"""
        red = f"{self.get_command()} 0x1111111111"
        green = f"{self.get_command()} 0x2222222222"
        if self.value == 100:
            self.value = 0
            self.trigger_command(red.split(sep=" "))
        else:
            self.value = 100
            self.trigger_command(green.split(sep=" "))
