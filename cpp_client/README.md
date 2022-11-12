# Turbo IoT Client
This part of the repository contains all of the C++ code.

## Developer Instructions
If you are working on this codebase as a developer, please modify your git configuration to change the default githook location. This can be done with the following shell command: `git config core.hooksPath .githooks`
To be able to use this githook you also need to install clang-format:
On Fedora: `sudo dnf install clang-tools-extra`
On Ubuntu: `sudo apt install clang-format`

# Get started

## On Host

The project contains a Makefile that can handle a lot of things for you. Most make commands
will automatically run in a custom docker container that is hosted and automatically build on gitlab.
The container is defined by cpp_client/Docker_ci_cpp. Note however that only when a change on the dockerfile
is pushed to main, it will rebuild the container on gitlab. So if you want to use your local changes to 
the docker file to build and test you must build one locally first. How to to this, it listed below with all the other make commands.

All of the following commands automatically run in a docker container that is hosted on gitlab:

    make (build/all)                    compiles the main application
    make start                          runs the main application
    make unit TEST="Your test here"     runs a specific unit test eg TEST=device runs test_device.cpp
    make unit-all                       runs all unit tests
    make integration                    runs the integration tests
    make test                           runs all tests (with valgrind)
    make clean                          removes build files
    make format                         applies clang formatting
    make format-check                   checks formatting


Use the following command to build a custom container using de cpp_client/Dockerfile_ci_cpp:

    make docker-build IMAGE_TAG=your-custom-tag

Use the following command to build and test code in the docker container you build in the previous step:

    make % IMAGE_TAG=your-custom-tag        where the % can be any make command listed above



