import asyncio
import json
import logging
from typing import List

import requests
from requests.exceptions import Timeout

from turbo_client.alarm import Alarm
from turbo_client.message import Message
from turbo_client.sensors.cpu_temp_sensor import CpuTemperatureSensor
from turbo_client.sensors.hardware_sensors.ambient_click_sensor import (
    AmbientClickSensor,
)
from turbo_client.sensors.hardware_sensors.key_click_sensor import (
    KeyClickSensor,
)
from turbo_client.sensors.hardware_sensors.thermo_click_sensor import (
    ThermoClickSensor,
)
from turbo_client.sensors.sensor import Sensor
from turbo_client.sensors.storage_sensor import StorageSensor
import subprocess

logger = logging.getLogger("main")


class Device:
    server_url: str
    device_id: str
    token: str
    sensor_a: List[
        Sensor
        | KeyClickSensor
        | ThermoClickSensor
        | CpuTemperatureSensor
        | StorageSensor
        | AmbientClickSensor
    ]
    """ frequency in seconds"""
    frequency: int

    def __init__(
        self, host: str, port: int, device_id: str, token: str, frequency: int
    ):
        self.server_url = f"http://{host}:{port}"

        self.device_id = device_id
        self.token = token
        self.sensor_a = list()
        self.frequency = frequency

    def get_headers(self):
        headers = dict()
        auth = f"Bearer {self.token}"
        headers["Authorization"] = auth
        return headers

    @classmethod
    def init_from_dict(cls, config: dict):
        dev = cls(
            host=config["server"]["hostname"],
            port=int(config["server"]["port"]),
            device_id=config["device"]["device_id"],
            token=config["device"]["token"],
            frequency=int(config["device"]["frequency"]),
        )
        if "sensors" in config["device"]:
            # fmt: off
            sensor: Sensor \
                    | KeyClickSensor \
                    | ThermoClickSensor \
                    | CpuTemperatureSensor \
                    | StorageSensor \
                    | AmbientClickSensor \
                    | None = (
                        None
                        )
            # fmt: on

            for sensor_dict in config["device"]["sensors"]:
                match sensor_dict["type"]:
                    case "click":
                        sensor = KeyClickSensor(
                            sensor_dict["sensor_id"],
                            int(sensor_dict["frequency_s"]),
                            int(sensor_dict["button_index"]),
                        )
                    case "temp":
                        sensor = ThermoClickSensor(
                            sensor_dict["sensor_id"],
                            int(sensor_dict["frequency_s"]),
                        )
                    case "cpu-temp":
                        sensor = CpuTemperatureSensor(
                            sensor_dict["sensor_id"],
                            int(sensor_dict["frequency_s"]),
                        )
                    case "storage":
                        sensor = StorageSensor(
                            sensor_dict["sensor_id"],
                            int(sensor_dict["frequency_s"]),
                        )
                    case "ambient":
                        sensor = AmbientClickSensor(
                            sensor_dict["sensor_id"],
                            int(sensor_dict["frequency_s"]),
                        )
                # add alarms to each sensor
                print(sensor_dict["alarms"])
                if "alarms" in sensor_dict and sensor is not None:
                    sensor.define_alarms_from_dict(sensor_dict["alarms"])
                if sensor is not None:
                    dev.sensor_a.append(sensor)
        return dev

    def server_is_reachable(self):
        request_path = "/api/service/ping"
        request_url = self.server_url + request_path

        headers = self.get_headers()

        logger.debug("server_is_reachable: GET %s", request_url)
        result = requests.get(request_url, headers=headers, timeout=1)

        if result.status_code == 200:
            return True
        logger.warning(
            "server_is_reachable: Error code %d: %s",
            result.status_code,
            result.text,
        )
        return False

    def fetch_properties_from_server(self) -> dict:
        """
        Returns:
            Dict with all properties (desired and reported)

        Raises:
            Connection error when it cannot reach server
        """
        request_path = f"/api/devices/{self.device_id}/properties"
        request_url = self.server_url + request_path

        headers = self.get_headers()
        logger.debug("fetch_properties_from_server: GET %s", request_url)
        params = {"device_id": self.device_id}
        result = requests.get(
            request_url, headers=headers, params=params, timeout=1
        )

        if result.status_code == 200:
            return result.json()
        logger.error(
            "fetch_properties_from_server: Error code %d: %s",
            result.status_code,
            result.text,
        )
        raise ConnectionError(
            f"fetch_properties_from_server: {result.status_code},{result.text}"
        )

    def get_desired_property(self, key: str) -> int | float | str | bool:
        """
        Parameters:
            key of the property to be fetched, only desired properties

        Returns:
            value with respect to the desired key

        Raises:
            Value Error if
        """
        data = self.fetch_properties_from_server()
        return data["desired"][key]

    def patch_reported_properties(self, patch: dict):
        """
        Parameters:
            Properties to be reported

        Raises:
            Connection Error if can't reach server
        """
        request_path = f"/api/devices/{self.device_id}/properties"
        request_url = self.server_url + request_path
        headers = self.get_headers()

        data = self.fetch_properties_from_server()
        for key in patch:
            if "reported" not in data:
                data["reported"] = {f"{key}": patch[key]}
            else:
                data["reported"][key] = patch[key]

        logger.debug(
            "patch_reported_properties: PUT %s, args %s", request_url, data
        )
        result = requests.put(
            request_url, headers=headers, json=data, timeout=1
        )

        if result.status_code != 200:
            logger.error(
                "patch_reported_properties: Error code %d: %s",
                result.status_code,
                result.text,
            )
            raise ConnectionError(
                f"patch_reported_properties:{result.status_code},{result.text}"
            )

    def send_message(self, payload: str):
        logger.info("send_message: sending message: %s", payload)
        resource_path = (
            f"{self.server_url}/api/devices/" f"{self.device_id}/messages"
        )
        data_dict = {"device_id": self.device_id, "payload": payload}
        logger.debug("send_message: POST %s", resource_path)
        result = requests.post(
            resource_path,
            data=data_dict,
            headers=self.get_headers(),
            timeout=1,
        )

        if (
            result.status_code != 200
            and result.status_code != 201
            and result.status_code != 204
        ):
            logger.error(
                "send_message: Error code %d: %s",
                result.status_code,
                result.text,
            )
            raise ConnectionError(
                f"send_message: {result.status_code}, {result.text}"
            )

    def receive_messages(self) -> List[Message]:
        resource_path = (
            f"{self.server_url}/api/devices/" f"{self.device_id}/messages"
        )
        logger.debug("receive_messages: GET %s", resource_path)
        result = requests.get(
            resource_path, headers=self.get_headers(), timeout=1
        )

        if result.status_code != 200:
            logger.error(
                "receive_messages: Error code %d: %s",
                result.status_code,
                result.text,
            )
            raise ConnectionError(
                f"receive_messages: {result.status_code}, {result.text}"
            )

        data = json.loads(result.text)
        returnlist = []
        for msgdict in data:
            logger.info("receive_messages: received: %s", msgdict)
            returnlist.append(Message.create_from_dict(msgdict))

        return returnlist

    def add_sensor(
        self,
        sensor: Sensor
        | KeyClickSensor
        | ThermoClickSensor
        | CpuTemperatureSensor
        | StorageSensor
        | AmbientClickSensor
        | None,
    ):
        """
        This function adds the sensor to device.sensor_a,
        and to the server, when the sensor is present on the
        server already it is still added to the list and vise versa

        Args:
            sensor (Sensor): the sensor to add

        Raises:
            ConnectionError: thrown when the server does not respond correctly
        """
        if sensor is None:
            return
        logger.info("add_sensor: adding sensor %s", sensor.sensor_id)
        resource_path = (
            f"{self.server_url}/api/devices/{self.device_id}/sensors"
        )
        data = {"sensor_id": sensor.sensor_id}
        logger.debug("add_sensor: POST %s", resource_path)
        result = requests.post(
            resource_path, headers=self.get_headers(), json=data, timeout=1
        )

        if result.status_code == 200:
            logger.info(
                "add_sensor: sensor %s posted to server",
                sensor.sensor_id,
            )
            # Sensor is in server: was created
            if sensor not in self.sensor_a:
                self.sensor_a.append(sensor)
            else:
                logger.warning(
                    "add_sensor: sensor %s already present on device",
                    sensor.sensor_id,
                )
            return
        elif result.status_code == 404:
            data = result.json()
            if (
                data["error"]
                == f"device sensor id {sensor.sensor_id} already registered"
            ):
                logger.warning(
                    "add_sensor: sensor %s was already registered",
                    sensor.sensor_id,
                )
                # Sensor was already in server: just add it to the list
                if sensor not in self.sensor_a:
                    self.sensor_a.append(sensor)
                else:
                    logger.warning(
                        "add_sensor: sensor %s already present on device",
                        sensor.sensor_id,
                    )
                return
        # something went wrong
        logger.error(
            "add_sensor: Error code %d: %s",
            result.status_code,
            result.text,
        )
        raise ConnectionError(
            f"add_sensor: {result.status_code}, {result.text}"
        )

    def remove_sensor(self, sensor_id: str):
        """
        Removes the sensor with sensor_id either from dev.sensor_a,
        from the server or both

        Args:
            sensor_id (str): id of the sensor to remove

        Raises:
            ConnectionError: thrown when the server does not respond correctly
        """
        logger.info("remove_sensor: removing sensor %s", sensor_id)

        resource_path = (
            f"{self.server_url}/api/devices/{self.device_id}"
            + f"/sensors/{sensor_id}"
        )
        logger.debug("add_sensor: DELETE %s", resource_path)
        result = requests.delete(
            resource_path, headers=self.get_headers(), timeout=1
        )

        self.sensor_a = [s for s in self.sensor_a if s.sensor_id != sensor_id]

        if result.status_code == 200:
            logger.info("remove_sensor: Sensor %s removed", sensor_id)
            return
        elif result.status_code == 404:
            data = result.json()
            if (
                data["error"]
                == f"device {self.device_id} sensor id {sensor_id}"
                + " not registered"
            ):
                logger.warning(
                    "remove_sensor: Sensor %s not registered", sensor_id
                )
                return
        logger.error(
            "remove_sensor: Error code %d: %s",
            result.status_code,
            result.text,
        )
        raise ConnectionError(
            f"remove_sensor: {result.status_code}, {result.text}"
        )

    def trigger_alarm(self, alarm: Alarm):
        logger.info("trigger_alarm: triggering alarm %s", alarm.alarm_id)
        alarm.triggered = True
        request_path = f"/api/devices/{self.device_id}/alarms"
        request_url = self.server_url + request_path
        alarm_data = {
            "alarm_id": alarm.alarm_id,
            "description": alarm.description,
        }
        logger.debug("trigger_alarm: POST %s", request_path)
        result = requests.post(
            request_url,
            headers=self.get_headers(),
            json=alarm_data,
            timeout=1,
        )
        if result.status_code == 400:
            logger.warning(
                "Error code %d: Alarm is already triggered", result.status_code
            )
            return
        if result.status_code != 201:
            logger.error(
                "trigger_alarm: Error code %d: %s",
                result.status_code,
                result.text,
            )
            raise ConnectionError(
                f"trigger alarm: {result.status_code} {result.text}"
            )

    def list_triggered_alarms(self) -> List[dict]:
        request_path = f"/api/devices/{self.device_id}/alarms"
        request_url = self.server_url + request_path

        logger.debug("list_triggered_alarms: GET %s", request_url)
        result = requests.get(
            request_url, headers=self.get_headers(), timeout=1
        )

        if result.status_code != 200:
            logger.error(
                "list_triggered_alarms: Error code %d: %s",
                result.status_code,
                result.text,
            )
            raise ConnectionError(
                f"list_triggered_alarms: {result.status_code} {result.text}"
            )

        return result.json()

    def resolve_alarm(self, alarm: Alarm):
        logger.info("resolve_alarm: resolving alarm %s", alarm.alarm_id)
        alarm.triggered = False
        request_path = f"/api/devices/{self.device_id}/alarms/{alarm.alarm_id}"
        request_url = self.server_url + request_path

        logger.debug("resolve_alarm: GET %s", request_url)
        result = requests.delete(
            request_url, headers=self.get_headers(), timeout=1
        )

        if result.status_code == 400:
            logger.debug(
                "resolve_alarm: Alarm %s already registered", alarm.alarm_id
            )
            return
        if result.status_code != 200:
            logger.error(
                "resolve_alarm: Error code %d: %s",
                result.status_code,
                result.text,
            )
            raise ConnectionError(
                f"resolve_alarm: {result.status_code} {result.text}"
            )

    def trigger_and_resolve_alarms(self):
        for sensor_i in self.sensor_a:
            value = sensor_i.value
            for alarm_i in sensor_i.alarm_a:
                over_treshold = alarm_i.rule.check_condition(value)
                if over_treshold and not alarm_i.triggered:
                    self.trigger_alarm(alarm_i)
                    alarm_i.trigger_actuators()
                elif not over_treshold and alarm_i.triggered:
                    self.resolve_alarm(alarm_i)
                    alarm_i.trigger_actuators()

    def post_sensor_readings(self) -> None:
        """
        For each sensor in the device, its latest value is posted to the server

        Raises:
            ConnectionError: if a post does not resturn status 200
        """
        resource_path_base = (
            f"{self.server_url}/api/devices/" + f"{self.device_id}/sensors/"
        )

        for sensor_i in self.sensor_a:
            resource_path = resource_path_base + sensor_i.sensor_id

            data = {sensor_i.get_state()["unit_str"]: sensor_i.value}
            logger.debug(
                "post_sensor_readings: PUT %s, data: %s", resource_path, data
            )
            result = requests.put(
                resource_path,
                headers=self.get_headers(),
                json=data,
                timeout=1,
            )

            if result.status_code != 200:
                logger.error(
                    "post_sensor_readings: Error code %d: %s",
                    result.status_code,
                    result.text,
                )
                raise ConnectionError(
                    f"post_sensor_readings: {result.status_code} {result.text}"
                )

    def setConnectionIndicator(self, state: int):
        """
        Sets a value to LED0 on the beaglebone by calling
        the nit_led_click driver

        Parameters:
            integer that is either 0 or 1

        """
        try:
            popen = subprocess.Popen(
                ["nit_led_click", "0", str(state)], stdout=subprocess.PIPE
            )
            popen.wait()
        except FileNotFoundError:
            logger.warning(
                "trigger_command: subprocess failed, %s",
                "you're probably not on the beagle",
            )

    async def poll_loop(self):
        await self.ping_unitil_reachable()
        await self.post_sensor_list()

        while True:
            try:
                logger.info("device %s is looping", self.device_id)
                self.receive_messages()
                self.post_sensor_readings()
                self.trigger_and_resolve_alarms()
                await asyncio.sleep(self.frequency)
            except (
                Timeout,
                requests.exceptions.ConnectionError,
                TimeoutError,
            ):
                await self.ping_unitil_reachable()

    async def post_sensor_list(self):
        for sensor_i in self.sensor_a:
            retry = True
            while retry:
                try:
                    self.add_sensor(sensor_i)
                    retry = False
                except (
                    Timeout,
                    requests.exceptions.ConnectionError,
                    TimeoutError,
                ):
                    await self.ping_unitil_reachable()

    async def ping_unitil_reachable(self):
        reachable = False
        while not reachable:
            self.setConnectionIndicator(0)
            try:
                reachable = self.server_is_reachable()
            except (
                Timeout,
                requests.exceptions.ConnectionError,
                TimeoutError,
            ):
                logger.warning("Server unreachable, ping until connection")
                await asyncio.sleep(self.frequency)

        self.setConnectionIndicator(1)
        return

    def init_poll_loop(self):
        """
        Inititializes the poll loop, creates a looping task for each sensor
        and the device, will repeat forever
        """
        logger.info("init_poll_loop: starting poll loops")

        loop = asyncio.get_event_loop()
        for sensor in self.sensor_a:
            task = loop.create_task(sensor.poll_loop())
            sensor.async_task = task
        loop.create_task(self.poll_loop())

        try:
            loop.run_forever()
        except asyncio.CancelledError:
            logger.error("Poll loop was cancelled")
