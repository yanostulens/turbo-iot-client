from datetime import datetime
import json
from dataclasses import dataclass, field


@dataclass(kw_only=True)
class Message:
    """
    Describes a message sent/received between the turbo-iot-client
    and turbo-iot-server

    Parameters:
        device_id:  ID of the device sending/receiving the message
        payload:    content of the message
        timestamp:  Optional: When the device sends a message no timestamp
                    attribute is used. The timestamp is generated inside
                    the server when the device receives a message the
                    timestamp is set to the server's generated timestamp.
                    All timestamps are in UTC format

    """

    device_id: str
    payload: str
    timestamp: str = field(default="")

    def __str__(self):
        if self.timestamp == "":
            as_string = f'by device_id "{self.device_id}": "{self.payload}"'
        else:
            as_string = (
                f'by device_id "{self.device_id}" at time'
                f' {self.timestamp}: "{self.payload}"'
            )
        return as_string

    @classmethod
    def create_from_dict(cls, data: dict):
        print(data)
        kwargs = dict()
        if "device_id" in data and len(data["device_id"]) <= 0:
            raise ValueError("device_id string is empty")
        if "device_id" in data and type(data["device_id"]) is not str:
            raise TypeError("device_id invalid: should be of type string")
        if "device_id" in data:
            kwargs["device_id"] = data["device_id"]

        kwargs["timestamp"] = ""
        if "timestamp" in data and type(data["timestamp"]) is not str:
            raise TypeError("timestamp invalid: should be of type string")
        if "timestamp" in data and len(data["timestamp"]) > 0:
            # if timestamp added as empty string: as if no timestamp
            try:
                _ = datetime.strptime(
                    data["timestamp"], "%Y-%m-%dT%H:%M:%S.%f"
                )
            except ValueError:
                raise ValueError(
                    "timestamp invalid: should be of format\
                        YYYY-MM-DDTHH:MM:SS.ssssss (%Y-%m-%dT%H:%M:%S.%f)"
                )
            else:
                kwargs["timestamp"] = data["timestamp"]

        if "payload" in data:
            if (
                type(data["payload"]) is not str
                and type(data["payload"]) is not dict
            ):
                raise TypeError(
                    "payload invalid: should be of type string or dict"
                )
            elif type(data["payload"]) == str and len(data["payload"]) <= 0:
                raise ValueError("payload string is empty")
            elif type(data["payload"]) == dict:
                kwargs["payload"] = json.dumps(data["payload"])
            else:  # is str
                kwargs["payload"] = data["payload"]
        return cls(**kwargs)
