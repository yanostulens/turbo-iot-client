import argparse
import json
import logging
from pathlib import Path

FREQUENCY = 5

logging.basicConfig(level=logging.WARNING)


def load_config(file: Path) -> dict:
    config: dict
    with open(file, "r") as config_file:
        config = json.load(config_file)

    if (
        "server" not in config
        or "port" not in config["server"]
        or "hostname" not in config["server"]
    ):
        raise IOError(
            "Server is not specified in the config file. "
            "Please specify port and hostname."
        )
    if (
        "device" not in config
        or "device_id" not in config["device"]
        or "token" not in config["device"]
    ):
        raise IOError(
            "Device is not specified in the config file. "
            "Please specify device_id and token."
        )
    if "sensors" not in config["device"]:
        logging.warning("No sensors are registered to the device.")
    if "frequency" not in config["device"]:
        config["device"]["frequency"] = FREQUENCY
        logging.warning(
            "Device polling frequency is not specified in the "
            "config file. Default frequency of "
            "{} will be set.".format(FREQUENCY)
        )
    return config


def parse_opts():
    main_parser = argparse.ArgumentParser()
    main_parser.add_argument(
        "--verbose",
        "-v",
        required=False,
        action="count",
        default=0
    )
    main_parser.add_argument(
        "--config",
        type=str,
        required=False,
        help="Path to the config file to use"
    )
    main_parser.add_argument(
        "--host",
        type=str,
        required=False,
        help="Hostname to the turbo-iot server to use. "
        "Overwrites the server url specified in the config file.",
    )
    main_parser.add_argument(
        "--port",
        type=int,
        required=False,
        help="HTTP port of the turbo-iot server to use. "
        "Overwrites the port specified in the config file.",
    )
    main_parser.add_argument(
        "--deviceid",
        type=str,
        required=False,
        help="Device ID to use. "
        "Overwrites the server url specified in the config file.",
    )
    main_parser.add_argument(
        "--token",
        type=str,
        required=False,
        help="Access token that is used to authenticate the device.\
        Overwrites the server url specified in the config file.",
    )
    main_parser.add_argument(
        "--frequency",
        type=int,
        required=False,
        help="Frequency on which the device will communicate with the server",
    )

    return main_parser.parse_args()


def overwrite_config(args, config: dict):
    if args.host is not None:
        config["server"]["hostname"] = args.host
    if args.port is not None:
        config["server"]["port"] = args.port
    if args.deviceid is not None:
        config["device"]["device_id"] = args.deviceid
    if args.token is not None:
        config["device"]["token"] = args.token
    if args.frequency is not None:
        config["device"]["frequency"] = args.frequency
    if args.verbose is not None:
        config["device"]["verbose"] = args.verbose

    return config


def process_inputs(config_path: Path) -> dict:
    config = load_config(config_path)
    args = parse_opts()
    return overwrite_config(args, config)
