# Turbo IoT Client
This repo has a fully implemented, runnable and tested 
Turbo IoT client through combination of code from trainee repositories.

# Get started

## On Target

Powering on the beaglebone starts the turbo client application.

## On Host

The project contains a Makefile that can handle a lot of things for you. The following make
commands are available:

    make                initializes the python virtual environment
    make dev            installs the dev requirements
    make dev_freeze     runs pip-compile with the dev requirement files
    make start          runs main.py in the venv
    make test           runs the unit tests
    make test_report    runs the unit tests and generates a coverage report
    make lint           lints the source code in src/ with flake8
    make typing         runs the mppy typechecker for the source code in src/
    make freeze         runs pip-compile with the correct requirement files
    make clean          cleans out the venv, cache and reports

# Documentation and test coverage

Visit the following [website](https://nalys-nit.gitlab.io/traineeship/2022-ist-fall/turbo-iot-client/) for up to date documentation and test coverage reports.

# Conventions

* keep your virtual environments up to date with pip compile
* When assinged a feature you will provide: Code, Tests (unit, integration), documentation where necessary
* "CI is source of truth" (Tim 2022)

# Checklist

* [x] class Device(Protocol):
  * [x] def server_is_reachable(): ...
  * [x] def fetch_properties_from_server(): ...
  * [x] def get_desired_property(key: str) -> int|float|str|bool: ...
  * [x] def patch_reported_properties(patch: dict): ...
  * [x] def send_message(payload: str): ...
  * [x] def receive_messages() -> List(Message): ...
  * [x] def add_sensor(sensor: DerivedSensor): ...
  * [x] def trigger_alarm(alarm: Alarm): ...
  * [x] def resolve_alarm(alarm_id: str): ...
  * [x] def list_triggered_alarms() -> List(Alarm): ...
* [x] class Sensor(Protocol):
  * [x] def get_value() -> int: ...
  * [x] det get_state() -> dict: ...
  * [x] def define_alarm(rule: AlarmdRule): ...
  * [x] def undefine_alarm(id): ...
* [x] a concrete sensor class e.g. cpu sensor
* [x] class Alarm():
* [x] class AlarmRule():
* [x] Arg parsing
* [x] Config parsing
* [x] Poll loop
* [x] Authentication
  * [x] token

* [x] put a readme
* [x] a Makefile

* [x] static test
  * [x] Linting
  * [x] Typechecking

* [x] Unit Tests:
  * [x] class Device(Protocol):
    * [x] def server_is_reachable(): ...
    * [x] def fetch_properties_from_server(): ...
    * [x] def get_desired_property(key: str) -> int|float|str|bool: ...
    * [x] def patch_reported_properties(patch: dict): ...
    * [x] def send_message(payload: str): ...
    * [x] def receive_messages() -> List(Message): ...
    * [x] def add_sensor(sensor: DerivedSensor): ...
    * [x] def trigger_alarm(alarm: Alarm): ...
    * [x] def resolve_alarm(alarm_id: str): ...
    * [x] def list_triggered_alarms() -> List(Alarm): ...

  * [x] class Message():

  * [x] class Sensor(Protocol):
    * [x] def get_value() -> int: ...
    * [x] det get_state() -> dict: ...
    * [x] def define_alarm(rule: AlarmdRule): ...
    * [ ] def undefine_alarm(id): ...

  * [x] a concrete sensor class e.g. cpu sensor

  * [x] class Alarm():
  * [x] class AlarmRule():

* [ ] Deployment:
  * [ ] Docker
  * [ ] VM
  * [x] Package
