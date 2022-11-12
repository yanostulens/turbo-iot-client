import json

import pytest
import requests_mock
from turbo_client.alarm import Alarm
from turbo_client.alarm_rule import AlarmRule, Condition
from turbo_client.device import Device

condition = Condition.GREATER_THAN
alarm_rule_1 = AlarmRule(condition, 20)
alarm_rule_2 = AlarmRule(condition, 30)

test_data = [
    (
        "localhost",
        8000,
        "Test device",
        "asdsfasdfasdfasfa",
        "sensor1",
        "alarm_01",
        "Fire alarm",
        "sensor1",
        alarm_rule_1,
    ),
    (
        "10.32.10.59",
        8000,
        "Test device on server",
        "asdsfasdfasdfasfa",
        "sensor2",
        "alarm_02",
        "temp alarm",
        "sensor2",
        alarm_rule_2,
    ),
]


@pytest.fixture
@pytest.mark.parametrize(
    "host,port,device_id,token,sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def config(
    host,
    port,
    device_id,
    token,
    sensor_id,
    alarm_id,
    description,
    origin,
    rule,
):
    config = dict()
    config["server"] = {"hostname": host}
    config["server"]["port"] = port
    config["device"] = {"device_id": device_id}
    config["device"]["token"] = token
    config["device"]["frequency"] = 5
    config["device"]["sensor"] = {"sensor_id": sensor_id}
    config["device"]["sensor"]["alarms"] = {"alarm_id": alarm_id}
    config["device"]["sensor"]["alarms"]["description"] = description
    config["device"]["sensor"]["alarms"]["origin"] = origin
    config["device"]["sensor"]["alarms"]["rule"] = rule
    return config


@pytest.fixture
@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def device(config):
    return Device.init_from_dict(config)


@pytest.fixture
@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def alarm(config):
    alarm = config["device"]["sensor"]["alarms"]
    return Alarm(
        alarm["alarm_id"],
        alarm["description"],
        alarm["origin"],
        alarm["rule"],
    )


@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_device_init_from_dict(host, port, device_id, token, config):
    device = Device.init_from_dict(config)

    assert (
        device.server_url == f"http://{host}:{port}"
        and device.device_id == device_id
        and device.token == token
    )


status_code = [200, 201, 404, 401, 1000]


@pytest.mark.parametrize("status_code", status_code)
@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_server_is_reachable(device, status_code):
    with requests_mock.Mocker() as mock:
        request_path = "/api/service/ping"
        request_url = device.server_url + request_path
        text = "pong"
        mock.get(request_url, text=text, status_code=status_code)
        if status_code == 200:
            assert device.server_is_reachable()
        else:
            assert not device.server_is_reachable()


@pytest.mark.parametrize("status_code", status_code)
@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_fetch_properties_from_server(device, status_code):
    with requests_mock.Mocker() as mock:
        request_path = f"/api/devices/{device.device_id}/properties"
        request_url = device.server_url + request_path
        text = '{"Able to fetch properties": "fun"}'
        mock.get(request_url, text=text, status_code=status_code)
        if status_code == 200:
            device.fetch_properties_from_server()
        else:
            with pytest.raises(ConnectionError):
                device.fetch_properties_from_server()


@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_get_desired_property(device):
    with requests_mock.Mocker() as mock:
        request_path = f"/api/devices/{device.device_id}/properties"
        request_url = device.server_url + request_path
        properties = '{"desired": {"version": 10}}'
        text = properties
        status_code = 200
        mock.get(request_url, text=text, status_code=status_code)
        assert device.get_desired_property("version") == 10


patch_key = [("version"), ("new_property")]


properties = [
    ({"desired": {"version": 10}}),
    ({"desired": {"version": 10}, "reported": {"version": 20}}),
]


@pytest.mark.parametrize("properties", properties)
@pytest.mark.parametrize("patch_key", patch_key)
@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_patch_reported_properties(device, patch_key, properties):
    with requests_mock.Mocker() as mock:
        request_path = f"/api/devices/{device.device_id}/properties"
        request_url = device.server_url + request_path
        text = json.dumps(properties)
        status_code = 200
        mock.get(request_url, text=text, status_code=status_code)
        mock.put(request_url, status_code=status_code)
        patch = {f"{patch_key}": 1000}
        device.patch_reported_properties(patch)

        history = mock.request_history
        post_payload = history[1].json()
        assert post_payload["reported"][f"{patch_key}"] == 1000


@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_trigger_alarm(device, alarm):
    with requests_mock.Mocker() as mock:
        request_path = f"/api/devices/{device.device_id}/alarms"
        request_url = device.server_url + request_path
        status_code = 201
        mock.post(request_url, status_code=status_code)
        device.trigger_alarm(alarm)

        history = mock.request_history
        post_payload = history[0].json()
        assert post_payload["alarm_id"] == alarm.alarm_id


@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_list_alarms(device):
    with requests_mock.Mocker() as mock:
        request_path = f"/api/devices/{device.device_id}/alarms"
        request_url = device.server_url + request_path
        status_code = 200
        text = [
            {"alarm_id": "alarm1", "description": "des"},
            {"alarm_id": "alarm2", "description": "des2"},
        ]
        mock.get(request_url, json=text, status_code=status_code)
        assert device.list_triggered_alarms() == text


@pytest.mark.parametrize(
    "host,port,device_id,token, sensor_id, alarm_id, description, origin, rule",
    test_data,
)
def test_resolve_alarm(device, alarm):
    with requests_mock.Mocker() as mock:
        request_path = (
            f"/api/devices/{device.device_id}/alarms/{alarm.alarm_id}"
        )
        request_url = device.server_url + request_path
        status_code = 200
        mock.delete(request_url, status_code=status_code)
        device.resolve_alarm(alarm)
