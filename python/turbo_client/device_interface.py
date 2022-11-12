from typing import Protocol


class DeviceInterface(Protocol):
    def server_is_reachable(self):
        ...

    def fetch_properties_from_server(self) -> dict:
        ...

    def get_desired_property(self, key: str) -> int | float | str | bool:
        ...

    def patch_reported_properties(self, patch: dict):
        ...
