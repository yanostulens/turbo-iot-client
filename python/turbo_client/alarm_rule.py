from dataclasses import dataclass
from enum import Enum, auto


class Condition(Enum):
    EQUALS = auto()
    GREATER_THAN = auto()
    SMALLER_THAN = auto()


@dataclass
class AlarmRule:
    condition: Condition
    treshold: int

    def check_condition(self, value) -> bool:
        match self.condition:
            case Condition.EQUALS:
                return value == self.treshold
            case Condition.GREATER_THAN:
                return value > self.treshold
            case Condition.SMALLER_THAN:
                return value < self.treshold
            case _:
                return False
