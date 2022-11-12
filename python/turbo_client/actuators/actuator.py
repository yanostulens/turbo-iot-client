import logging
import subprocess
from abc import ABC, abstractmethod
from typing import List

logger = logging.getLogger("main")


class Actuator(ABC):
    actuator_id: str
    frequency: int

    def __init__(self, actuator_id: str) -> None:
        self.actuator_id = actuator_id

    @abstractmethod
    def trigger(self) -> None:
        pass

    def trigger_command(self, cmd: List[str]):
        try:
            popen = subprocess.Popen(cmd, stdout=subprocess.PIPE)
            popen.wait()
        except FileNotFoundError:
            logger.warning(
                "trigger_command: subprocess failed, %s",
                "you're probably not on the beagle",
            )
