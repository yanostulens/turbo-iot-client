#pragma once

#include <string>

class DriverHandlerInterface {
    public:
        virtual void setBargraph(const std::string& value) = 0;
        virtual void setBuzzer(const int8_t value) = 0;

        virtual int get2x2Click(const int8_t button_index) = 0;
        virtual float getTemperature() = 0; 
};

