#include <lib/drivers/include/driver_handler.hpp>

void DriverHandler::setBargraph(const std::string &) { return; }

void DriverHandler::setBuzzer(const int8_t) { return; }

int DriverHandler::get2x2Click(const int8_t) { return (int8_t)1; }

float DriverHandler::getTemperature() { return (float)20.0; }
