from turbo_client.actuators.actuator import Actuator


class BuzzerActuator(Actuator):
    index: int
    value: int

    def __init__(self, actuator_id: str):
        Actuator.__init__(self, actuator_id)
        self.value = 0

    def get_command(self) -> str:
        return "nit_buzzer_click"

    def trigger(self) -> None:
        if self.value == 100:
            self.value = 0
        else:
            self.value = 100

        cmd_args = f"{self.get_command()} {self.value}"
        self.trigger_command(cmd_args.split(sep=" "))
