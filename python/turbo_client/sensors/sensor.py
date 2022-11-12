import asyncio
import logging
from abc import ABC, abstractmethod
from typing import List

from turbo_client.alarm import Alarm
from turbo_client.alarm_rule import AlarmRule, Condition

logger = logging.getLogger()


class Sensor(ABC):
    sensor_id: str
    frequency: int
    value: int
    alarm_a: List[Alarm]

    def __init__(self, sensor_id: str, frequency: int) -> None:
        self.sensor_id = sensor_id
        self.frequency = frequency
        self.get_value()
        self.alarm_a = list()

    @abstractmethod
    def get_value(self) -> None:
        pass

    @abstractmethod
    def get_state(self) -> dict:
        pass

    def define_alarm(
        self, alarm_id: str, descr: str, rule: AlarmRule
    ) -> Alarm | None:
        new_alarm = Alarm(alarm_id, descr, self.sensor_id, rule)
        if new_alarm not in self.alarm_a:
            self.alarm_a.append(new_alarm)
            logger.info(
                "alarm %s added to sensor %s", alarm_id, self.sensor_id
            )
            return new_alarm
        return None

    def define_alarms_from_dict(self, config: List[dict]):
        for alarm_dict in config:
            condition = None
            match alarm_dict["rule"]["condition"]:
                case "=":
                    condition = Condition.EQUALS
                case ">":
                    condition = Condition.GREATER_THAN
                case "<":
                    condition = Condition.SMALLER_THAN
            if condition is not None:
                alarm = self.define_alarm(
                    alarm_dict["alarm_id"],
                    alarm_dict["description"],
                    AlarmRule(condition, alarm_dict["rule"]["threshold"]),
                )
                if alarm is not None and "actuators" in alarm_dict:
                    actuators = alarm_dict["actuators"]
                    print(actuators)
                    alarm.add_actuators_from_dict(actuators)

    def undefine_alarm(self, alarm_id: str) -> bool:
        for i in self.alarm_a:
            if i.alarm_id == alarm_id:
                self.alarm_a.remove(i)
                return True
        return False

    async def poll_loop(self):
        while True:
            self.get_value()
            logger.info(
                "sensor %s read in value %d", self.sensor_id, self.value
            )
            logger.debug("poll_loop: sensor %s is looping", self.sensor_id)
            await asyncio.sleep(self.frequency)
