from turbo_client.actuators.actuator import Actuator


class SegClickActuator(Actuator):
    value: int

    def __init__(self, actuator_id: str):
        Actuator.__init__(self, actuator_id)
        self.value = 0

    def get_command(self) -> str:
        return "nit_7seg_click"

    def trigger(self) -> None:
        # TODO
        cmd_args = f"{self.get_command()} {10}"
        self.trigger_command(cmd_args.split(" "))
