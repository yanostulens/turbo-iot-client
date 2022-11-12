#include <fcntl.h>
#include <lib/drivers/include/bargraph.hpp>
#include <lib/drivers/include/gpioexception.hpp>
#include <sys/stat.h>
#include <unistd.h>

void Bargraph::setBargraph(const std::string &value) {
  if (value.length() != 12) {
    GpioException("Usage: %s <0x0000000001>");
  }
  struct gpiod_line_bulk gpiod_ctx;

  initGpios(&gpiod_ctx);
  enableDisplay();
  setOutput(&gpiod_ctx, value.substr(2));
}

void Bargraph::initGpios(struct gpiod_line_bulk *gpiod_ctx) {
  struct gpiod_chip *chip;

  if ((chip = gpiod_chip_open_by_name(CHIPNAME)) == NULL) {
    throw GpioException("Open gpiochip failed");
  }

  if (gpiod_chip_get_lines(chip, gpio_pins, 20, gpiod_ctx) != 0) {
    throw GpioException("Get line failed");
  }

  if (gpiod_line_request_bulk_output(gpiod_ctx, "Consumer", 0) != 0) {
    throw GpioException("Request output failed");
  }
}

void Bargraph::setOutput(gpiod_line_bulk *gpiod_ctx, const std::string &input) {
  int values[20] = {0};
  for (uint32_t i = 0; i < input.length(); i++) {
    switch (input[i]) {
    case '0':
      values[i] = 0;
      values[i + 10] = 0;
      break;
    case '1':
      values[i] = 1;
      values[i + 10] = 0;
      break;
    case '2':
      values[i] = 0;
      values[i + 10] = 1;
      break;
    case '3':
      values[i] = 1;
      values[i + 10] = 1;
      break;
    default:
      throw GpioException("setOutput failed");
    }
  }
  gpiod_line_set_value_bulk(gpiod_ctx, values);
}

void Bargraph::enableDisplay() {
  int fd;
  char buffer[BUFFER_SIZE];

  if ((fd = open("/sys/class/gpio/export", O_WRONLY)) < 0) {
    throw GpioException("Unable to open /sys/class/gpio/export");
  }

  // Check if already exported
  snprintf(buffer, BUFFER_SIZE, "/sys/class/gpio/gpio%d", ENABLE_GPIO);
  if (access(buffer, F_OK) == 0) {
    return;
  }

  // Export if needed
  int len = snprintf(buffer, BUFFER_SIZE, "%d\n", ENABLE_GPIO);
  if (write(fd, buffer, len) != len) {
    throw GpioException("Unable to write to /sys/class/gpio/export");
  }
  close(fd);

  // Set output mode
  snprintf(buffer, BUFFER_SIZE, "/sys/class/gpio/gpio%d/direction",
           ENABLE_GPIO);
  if ((fd = open(buffer, O_WRONLY)) < 0) {
    throw GpioException("Unable to open /sys/class/gpio/gpioX/direction");
  }

  if (write(fd, "out\n", 4) != 4) {
    throw GpioException("Unable to write to /sys/class/gpio/gpioX/direction");
  }
  close(fd);

  // Set output value
  snprintf(buffer, BUFFER_SIZE, "/sys/class/gpio/gpio%d/value", ENABLE_GPIO);
  if ((fd = open(buffer, O_WRONLY)) < 0) {
    throw GpioException("Unable to open /sys/class/gpio/gpioX/value");
  }

  if (write(fd, "1\n", 2) != 2) {
    throw GpioException("Unable to write to /sys/class/gpio/gpioX/value");
  }
  close(fd);
}
