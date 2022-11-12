#include <lib/drivers/include/frequency_exception.hpp>
#include <lib/drivers/include/gpioexception.hpp>
#include <lib/drivers/include/new_buzzer.hpp>

#define BUZZER_GPIO 51
#define GPIO_OFFSET 32
#define CONSUMER "Consumer"
#define CHIPNAME "gpiochip1"
#define TO_NANO_SECONDS 1000000000

NewBuzzer::NewBuzzer(int frequency_hz)
    : m_buzzer_state{0}, m_kill_buzzer{false}, m_frequency_hz{frequency_hz},
      m_buzzer_mutex{}, m_buzzer_thread{
                            std::thread(&NewBuzzer::buzzer_loop, this)} {};

NewBuzzer::~NewBuzzer() {
  m_kill_buzzer = true;
  m_buzzer_mutex.try_lock();
  m_buzzer_mutex.unlock();

  if (m_buzzer_thread.joinable()) {
    m_buzzer_thread.join();
  }
  if (m_line) {
    gpiod_line_release(m_line);
  }
  if (m_chip) {
    gpiod_chip_close(m_chip);
  }
};

void NewBuzzer::init_gpio() {
  int ret;
  if (m_frequency_hz < 100 || m_frequency_hz > 10000) {
    throw FrequencyException("Frequency should be between 100-10000 Hz");
  };

  int64_t period_ns = (1.0f / (float)m_frequency_hz) * TO_NANO_SECONDS;
  m_period_ns = std::chrono::nanoseconds{period_ns};

  m_chip = gpiod_chip_open_by_name(CHIPNAME);
  if (!m_chip) {
    throw GpioException("Failed to to open chip");
  }

  m_line = gpiod_chip_get_line(m_chip, BUZZER_GPIO - GPIO_OFFSET);
  if (!m_line) {
    throw GpioException("Failed to get line");
  }

  ret = gpiod_line_request_output(m_line, CONSUMER, 0);
  if (ret < 0) {
    throw GpioException("Failed to set line to output");
  }
}

void NewBuzzer::setBuzzer(const int8_t value) {
  if (m_buzzer_state == value) {
    return;
  }

  int check = (m_buzzer_state == 0) - (value == 0);

  // Buzzer is running and request to stop is made
  if (check == -1) {
    m_buzzer_mutex.lock();
    m_buzzer_state = value;
  }
  // Buzzer is not running, and thus the mutex is locked, a request to start the
  // buzzer is made though
  else if (check == 1) {
    m_buzzer_state = value;
    m_buzzer_mutex.unlock();
  }
}

void NewBuzzer::buzzer_loop() {
  while (!m_kill_buzzer) {
    gpiod_line_set_value(m_line, 1);
    std::this_thread::sleep_for(m_period_ns);
    gpiod_line_set_value(m_line, 0);
    std::this_thread::sleep_for(m_period_ns);

    if (m_buzzer_state == 0) {
      std::unique_lock<std::mutex> lock(m_buzzer_mutex);
    }
  }
}
