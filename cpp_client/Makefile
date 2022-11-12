IN_DOCKER = $(shell bash ./scripts/in_docker.sh)
CHECK_FORMAT = ./scripts/clang_format.sh
IMAGE_PATH = registry.gitlab.com/nalys-nit/traineeship/2022-ist-fall/turbo-iot-client/cpp-env
IMAGE_TAG ?= latest
DOCKER_FILE = ./docker/Dockerfile_ci_cpp
DOCKER_RUN = docker run -t --rm -v=$$(pwd):/work -w /work -v=$$HOME:/home/nit -u $$(id -u $${USER}):$$(id -g $${USER}) ${IMAGE_PATH}:${IMAGE_TAG}

UNIT_TESTS = test/unit_test
INTEGRATION_TESTS = test/integration_test
MOCK_INCLUDE = test/include/*.h*
MOCK_SOURCES = test/src/*.cpp
SOURCES = src/*.cpp
INCLUDE = include/*.h*
EXTERNAL_LIBRARIES = lib/*
BIN = bin
EXTRA_CXXFLAGS = -g -Wall -Wextra -Werror
LINKED_LIBRARIES = -lcpprest -lssl -lcrypto
DEFINITIONS = -DLOG_LEVEL=LOG_SEV_DEBUG
LINKED_TESTS = -lgtest -lgtest_main -lgmock

ifeq ($(IN_DOCKER),1)
.PHONY: all
all: build

.PHONY: format
format:
	@echo "Formatting the c++ files"
	find . -iname *.h -o -iname *.c -o -iname *.cpp -o -iname *.hpp -not -path "./lib/*" \
	| xargs clang-format -i

.PHONY: format-check
format-check:
	@${CHECK_FORMAT}

.PHONY: build
build: ${BIN}/turbo_client ${SOURCES} ${INCLUDE} ${EXTERNAL_LIBRARIES}
	@echo "The application has been built"

${BIN}/turbo_client: ${SOURCES} ${INCLUDE} ${EXTERNAL_LIBRARIES} Makefile
	@echo "Building the application"
	@mkdir -p ${BIN}
	${CXX} ${CXXFLAGS} ${EXTRA_CXXFLAGS} ${SOURCES} ${LINKED_LIBRARIES} ${DEFINITIONS} -I ./ -o ${BIN}/turbo_client

.PHONY: start
start: build
	./${BIN}/turbo_client

.PHONY: build-unit
build-unit: ${SOURCES} ${INCLUDE} ${EXTERNAL_LIBRARIES} ${MOCK_INCLUDE} ${MOCK_SOURCES}
	@mkdir -p ${BIN}/${UNIT_TESTS}
	$(eval SKIP_OWNED_MOCK = $(filter-out test/src/mock_${TEST}.cpp,$(wildcard test/src/*.cpp)))
	${CXX} ${CXXFLAGS} ${EXTRA_CXXFLAGS} src/*${TEST}.cpp ${SKIP_OWNED_MOCK} ${UNIT_TESTS}/test_${TEST}.cpp ${LINKED_LIBRARIES} ${LINKED_TESTS} -I ./ -o ${BIN}/${UNIT_TESTS}/${TEST}

.PHONY: unit
unit: build-unit ${SOURCES} ${INCLUDE} ${EXTERNAL_LIBRARIES} ${MOCK_INCLUDE} ${MOCK_SOURCES}
	./${BIN}/${UNIT_TESTS}/${TEST}

.PHONY: unit-all
unit-all: ${SOURCES} ${INCLUDE} ${EXTERNAL_LIBRARIES} ${MOCK_INCLUDE} ${MOCK_SOURCES}
	$(MAKE) build-unit TEST=actuator
	$(MAKE) build-unit TEST=alarm
	$(MAKE) build-unit TEST=alarmrule
	$(MAKE) build-unit TEST=config
	$(MAKE) build-unit TEST=device
	$(MAKE) build-unit TEST=hardware_actuator
	$(MAKE) build-unit TEST=sensor
	$(MAKE) unit TEST=actuator
	$(MAKE) unit TEST=alarm
	$(MAKE) unit TEST=alarmrule
	$(MAKE) unit TEST=config
	$(MAKE) unit TEST=device
	$(MAKE) unit TEST=hardware_actuator
	$(MAKE) unit TEST=sensor

.PHONY: integration
integration:
	@echo "Running integration tests"

.PHONY: debug
debug: ${BIN}/turbo_client
	gdb ./${BIN}/turbo_client

.PHONY: clean
clean:
	rm -rf ${BIN}
else
.PHONY: docker-build
docker-build:
	docker build -t ${IMAGE_PATH}:${IMAGE_TAG} . -f ${DOCKER_FILE}

%:
	@echo "Not in docker, setup virtual environment..."
	$(DOCKER_RUN) $(MAKE) $@ TEST=${TEST}
endif

	 
