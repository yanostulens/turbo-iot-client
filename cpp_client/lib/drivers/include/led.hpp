#pragma once
#include <lib/drivers/include/driver_handler.hpp>
#include <gpiod.h>

class Led: public DriverHandler {
    public:
        Led(const int8_t led_number);
        ~Led();
        void setLed(const int8_t led_state) override;
        void init_gpio();
    private:
        int8_t m_led_number;
        struct gpiod_chip* m_chip;
        struct gpiod_line* m_line;
};
