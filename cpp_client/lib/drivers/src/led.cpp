#include <lib/drivers/include/frequency_exception.hpp>
#include <lib/drivers/include/gpioexception.hpp>
#include <lib/drivers/include/led.hpp>

#define LED_GPIO_OFFSET 53
#define GPIO_LINE_OFFSET 32
#define CONSUMER "Consumer"
#define CHIPNAME "gpiochip1"

Led::Led(const int8_t led_number) : m_led_number{led_number} {}

Led::~Led() {
  std::cout << "In destructor" << std::endl;
  if (m_line) {
    gpiod_line_release(m_line);
  }
  if (m_chip) {
    gpiod_chip_close(m_chip);
  }
};

void Led::init_gpio() {
  int ret;

  if (m_led_number < 0 || m_led_number > 3) {
    throw GpioException("Led index should be between 0-3");
  }

  m_chip = gpiod_chip_open_by_name(CHIPNAME);
  if (!m_chip) {
    throw GpioException("Failed to to open chip");
  }

  m_line = gpiod_chip_get_line(
      m_chip, (LED_GPIO_OFFSET - GPIO_LINE_OFFSET + m_led_number));
  if (!m_line) {
    throw GpioException("Failed to get line");
  }

  ret = gpiod_line_request_output(m_line, CONSUMER, 0);
  if (ret < 0) {
    throw GpioException("Failed to set line to output");
  }
  std::cout << "Setup" << std::endl;
}

void Led::setLed(const int8_t led_state) {
  int ret;
  if (!(led_state == 0 || led_state == 1)) {
    throw GpioException("State to set to the led must be either 0 or 1");
  }
  ret = gpiod_line_set_value(m_line, 1);
  if (ret < 0) {
    throw GpioException("Failed to set led state");
  }
  std::cout << "Set led done" << std::endl;
}
