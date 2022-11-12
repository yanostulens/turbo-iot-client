#include <lib/drivers/include/gpioexception.hpp>
#include <lib/drivers/include/keyclick2x2.hpp>

#define ARRAY_LEN(array) (int)(sizeof((array)) / sizeof((array)[0]))

int KeyClick2x2::get2x2Click(const int8_t button_index) {
  if (button_index > 3) {
    throw GpioException("Button index out of range <0-3>");
  }
  return this->getValues().at(button_index);
}

std::vector<int> KeyClick2x2::getValues() {
  struct gpiod_chip *chip;
  struct gpiod_line *line;
  std::vector<int> toReturn{};

  for (int i = 0; i < ARRAY_LEN(buttons); i++) {
    if (buttons[i].offset < 0) {
      // Gpio not available. Just print 0
      toReturn.push_back(0);
      continue;
    }

    if ((chip = gpiod_chip_open_by_name(buttons[i].chip)) == NULL) {
      throw GpioException("Open gpiochip failed");
    }

    if ((line = gpiod_chip_get_line(chip, buttons[i].offset)) == NULL) {
      throw GpioException("Get gpio line failed");
    }

    if (gpiod_line_request_input(line, "Consumer") != 0) {
      throw GpioException("Get input failed");
    }

    toReturn.push_back(gpiod_line_get_value(line));

    gpiod_chip_close(chip);
  }

  return toReturn;
}

void KeyClick2x2::printValues() {
  auto values = getValues();
  for (int x : values) {
    std::cout << x << " ";
  }
  std::cout << std::endl;
}
