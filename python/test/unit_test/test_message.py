import pytest
from turbo_client.message import Message
from turbo_client.device import Device
import requests_mock

good_id = "device1"
good_payload = "Hello World!"
good_timestamp = "2022-09-27T07:16:25.373638"

good_dicts = [
    {
        "device_id": good_id,
        "payload": good_payload,
        "timestamp": good_timestamp,
    },
    {"device_id": good_id, "payload": good_payload, "timestamp": ""},
    {"device_id": good_id, "payload": good_payload},
]

value_error_dicts = [
    {
        "device_id": good_id,
        "payload": good_payload,
        "timestamp": "18 October 2022 12:48",
    },
    {"device_id": good_id, "payload": ""},
    {"device_id": "", "payload": good_payload},
]

type_error_dicts = [
    {"device_id": good_id, "payload": good_payload, "timestamp": 12.48},
    {"device_id": 1, "payload": good_payload},
    {"device_id": good_id, "payload": 404},
    {"device_id": good_id},
    {"payload": good_payload},
]

payloads = ["Hello World!", {"subject": "test", "content": "Hello World!"}]


@pytest.mark.parametrize("data_dict", value_error_dicts)
def test_create_from_dict_value_error(data_dict):
    with pytest.raises(ValueError):
        m = Message.create_from_dict(data_dict)  # noqa: F841


@pytest.mark.parametrize("data_dict", type_error_dicts)
def test_create_from_dict_type_error(data_dict):
    with pytest.raises(TypeError):
        m = Message.create_from_dict(data_dict)  # noqa: F841


@pytest.mark.parametrize("data_dict", good_dicts)
def test_create_from_dict_correctly(data_dict):
    m = Message.create_from_dict(data_dict)
    assert data_dict["device_id"] == m.device_id
    assert data_dict["payload"] == m.payload
    if "timestamp" in data_dict:
        assert data_dict["timestamp"] == m.timestamp


@pytest.mark.parametrize("data_dict", good_dicts)
def test_str(data_dict):
    m = Message.create_from_dict(data_dict)
    expected_str = f'by device_id "{data_dict["device_id"]}"'
    if "timestamp" in data_dict and len(data_dict["timestamp"]) > 0:
        expected_str = expected_str + f' at time {data_dict["timestamp"]}'
    expected_str = expected_str + f': "{data_dict["payload"]}"'

    assert str(m) == expected_str


@pytest.mark.parametrize("payload", payloads)
def test_send_message_success(payload):
    d = Device("localhost", 8000, "fake_device", "fake_token", 5)
    resource_path = "{}/api/devices/{}/messages".format(
        d.server_url, d.device_id
    )
    data_dict = {"device_id": d.device_id, "payload": payload}
    status_code = 200

    with requests_mock.Mocker() as mock:
        mock.post(resource_path, json=data_dict, status_code=status_code)
        d.send_message(payload)


@pytest.mark.parametrize("payload", payloads)
def test_send_message_failure(payload):
    d = Device("localhost", 8000, "fake_device", "fake_token", 5)
    resource_path = "{}/api/devices/{}/messages".format(
        d.server_url, d.device_id
    )
    data_dict = {"device_id": d.device_id, "payload": payload}
    status_code = 404

    with requests_mock.Mocker() as mock:
        mock.post(resource_path, json=data_dict, status_code=status_code)
        with pytest.raises(ConnectionError):
            d.send_message(payload)


def test_receive_message_success():
    d = Device("localhost", 8000, "fake_device", "fake_token", 5)
    resource_path = "{}/api/devices/{}/messages".format(
        d.server_url, d.device_id
    )
    status_code = 200

    json_data = [
        {
            "device_id": good_id,
            "payload": good_payload,
            "timestamp": good_timestamp,
        }
    ]

    with requests_mock.Mocker() as mock:
        mock.get(resource_path, json=json_data, status_code=status_code)
        d.receive_messages()


def test_receive_message_failure():
    d = Device("localhost", 8000, "fake_device", "fake_token", 5)
    resource_path = "{}/api/devices/{}/messages".format(
        d.server_url, d.device_id
    )
    status_code = 404

    with requests_mock.Mocker() as mock:
        mock.get(resource_path, status_code=status_code)
        with pytest.raises(ConnectionError):
            d.receive_messages()
