VENV = ".venv"
UNIT_TESTS = test/unit_test
INTEGRATION_TESTS = test/integration_test
MAIN = turbo_client/__main__.py
SOURCES = turbo_client/
REQUIREMENTS = requirements/requirements.txt
REQUIREMENTS-DEV = requirements-dev/requirements-dev.txt
PACKAGES = requirements/requirements.in
PACKAGES-DEV = requirements-dev/requirements-dev.in
PYTHON = python

${VENV}: ${REQUIREMENTS}
	python3 -m venv ${VENV}
	. ${VENV}/bin/activate && pip install --requirement ${REQUIREMENTS}

.PHONY: python_module
python_module: ${VENV} ${SOURCES}
	. ${VENV}/bin/activate && pip install -e ./

.PHONY: dev
dev: ${VENV} ${REQUIREMENTS-DEV}
	. ${VENV}/bin/activate && pip install --requirement ${REQUIREMENTS-DEV}

.PHONY: dev_freeze
dev_freeze: ${PACKAGES-DEV}
	pip-compile ${PACKAGES-DEV} -o ${REQUIREMENTS-DEV}

# Run all tests
.PHONY: test
test: unit integration

# Run integration tests
.PHONY: integration
integration: ${VENV} dev
	. ${VENV}/bin/activate && ${PYTHON} -m pytest --verbose --cov=turbo_client --cov-report html:reports/integration_coverage ${INTEGRATION_TESTS}

.PHONY: unit
unit: ${VENV} dev
	. ${VENV}/bin/activate && ${PYTHON} -m pytest --verbose --cov=turbo_client --cov-report html:reports/unit_coverage ${UNIT_TESTS}

.PHONY: lint
lint: ${VENV} dev
	. ${VENV}/bin/activate && flake8 ${SOURCES}

.PHONY: typing
typing: ${VENV} dev
	. ${VENV}/bin/activate && mypy ${SOURCES}

.PHONY: freeze
freeze: ${PACKAGES}
	pip-compile ${PACKAGES} -o ${REQUIREMENTS}
	pip-compile ${PACKAGES} -o ${REQUIREMENTS-DEV}

.PHONY: docs
docs:
	doxygen ./doxygen/Doxyfile

.PHONY: clean
clean:
	rm -rf ${VENV} .pytest_cache .coverage coverage/ turbo-iot-client.log && find . | grep -E "(__pycache__)" | xargs rm -rf

.PHONY: run
run: ${VENV} python_module
	. ${VENV}/bin/activate && python3 -m turbo_client

.PHONY: install
install:
	@mkdir -p $(DESTDIR)
	cp -rf ./turbo_client $(DESTDIR)
