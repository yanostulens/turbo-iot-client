import logging

from dataclasses import dataclass, field
from typing import List

from turbo_client.actuators.actuator import Actuator
from turbo_client.actuators.bargraph_click_actuator import (
    BarGraphClickActuator,
)
from turbo_client.actuators.buzzer_actuator import BuzzerActuator
from turbo_client.actuators.seg_click_actuator import SegClickActuator
from turbo_client.alarm_rule import AlarmRule

logger = logging.getLogger("main")


@dataclass
class Alarm:
    alarm_id: str
    description: str
    origin: str
    rule: AlarmRule
    timestamp: str = field(default="")
    triggered: bool = field(default=False)
    actuator_a: List[Actuator] = field(default_factory=list)

    def trigger_actuators(self):
        logger.info("trigger_actuators: for alarm %s", self.alarm_id)
        for actuator_i in self.actuator_a:
            actuator_i.trigger()

    def add_actuator(self, actuator: Actuator):
        if actuator not in self.actuator_a:
            self.actuator_a.append(actuator)

    def add_actuators_from_dict(self, config: List[dict]):
        print(config)
        for actuator_dict in config:
            # fmt: off
            actuator: BuzzerActuator\
                    | BarGraphClickActuator\
                    | SegClickActuator | None
            # fmt: on
            actuator = None
            match actuator_dict["type"]:
                case "buzzer":
                    actuator = BuzzerActuator(actuator_dict["actuator_id"])
                case "bargraph":
                    actuator = BarGraphClickActuator(
                        actuator_dict["actuator_id"]
                    )
                case "segclick":
                    actuator = SegClickActuator(actuator_dict["actuator_id"])
            if actuator is not None and actuator not in self.actuator_a:
                self.actuator_a.append(actuator)
                logger.info(
                    "actuator %s added to alarm %s",
                    actuator_dict["actuator_id"],
                    self.alarm_id,
                )
