#include <lib/drivers/include/driver_handler.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <gpiod.h>


class NewBuzzer: public DriverHandler {
    public:
        NewBuzzer(int frequency_hz);
        ~NewBuzzer();
        void setBuzzer(const int8_t value) override;
        void init_gpio();
    private:
        std::atomic<int8_t> m_buzzer_state;
        std::atomic<bool> m_kill_buzzer;
        int m_frequency_hz;
        std::mutex m_buzzer_mutex;        std::thread m_buzzer_thread;
        std::chrono::duration<int64_t, std::ratio<1, 1000000000>> m_period_ns;
        struct gpiod_chip* m_chip;
        struct gpiod_line* m_line;
        
        void buzzer_loop();
};
