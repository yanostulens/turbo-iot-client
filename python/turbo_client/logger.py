import logging
from turbo_client.config_parser import process_inputs
from pathlib import Path


config_path = Path.cwd().joinpath("config.json")
logging_path = Path("turbo-iot-client.log")


class Logger():
    @staticmethod
    def __create_logger() -> logging.Logger:
        mylogger = logging.getLogger("main")
        config = process_inputs(config_path)
        mylogger.setLevel(logging.DEBUG)

        fh = logging.FileHandler(logging_path, "w")
        fh.setLevel(logging.DEBUG)

        ch = logging.StreamHandler()
        match config["device"]["verbose"]:
            case 0:
                ch.setLevel(logging.ERROR)
            case 1:
                ch.setLevel(logging.WARNING)
            case 2:
                ch.setLevel(logging.INFO)
            case 3:
                ch.setLevel(logging.DEBUG)

        formatter = logging.Formatter(
            "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
        )
        fh.setFormatter(formatter)
        ch.setFormatter(formatter)

        mylogger.addHandler(fh)
        mylogger.addHandler(ch)
        mylogger.propagate = False

        return mylogger

    @staticmethod
    def get_logger():
        logger = Logger.__create_logger()
        return logger
