#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "mock_hal.h"
#include "v4/v4_hal.h"

TEST_CASE("Mock HAL - GPIO operations")
{
  mock_hal_reset();

  SUBCASE("Initialize GPIO pin")
  {
    v4_err err = v4_hal_gpio_init(5, V4_HAL_GPIO_MODE_OUTPUT);
    CHECK(err == 0);
    CHECK(mock_hal_gpio_get_mode(5) == V4_HAL_GPIO_MODE_OUTPUT);
  }

  SUBCASE("Write to GPIO pin")
  {
    v4_hal_gpio_init(7, V4_HAL_GPIO_MODE_OUTPUT);
    v4_err err = v4_hal_gpio_write(7, 1);
    CHECK(err == 0);
    CHECK(mock_hal_gpio_get_value(7) == 1);

    err = v4_hal_gpio_write(7, 0);
    CHECK(err == 0);
    CHECK(mock_hal_gpio_get_value(7) == 0);
  }

  SUBCASE("Read from GPIO pin")
  {
    v4_hal_gpio_init(3, V4_HAL_GPIO_MODE_INPUT);
    int value;
    v4_err err = v4_hal_gpio_read(3, &value);
    CHECK(err == 0);
  }

  SUBCASE("GPIO bounds check")
  {
    v4_err err = v4_hal_gpio_init(100, V4_HAL_GPIO_MODE_OUTPUT);
    CHECK(err == -13);  // OutOfBounds
  }
}

TEST_CASE("Mock HAL - UART operations")
{
  mock_hal_reset();

  SUBCASE("Initialize UART")
  {
    v4_err err = v4_hal_uart_init(0, 115200);
    CHECK(err == 0);
  }

  SUBCASE("Send character")
  {
    v4_hal_uart_init(0, 9600);
    v4_err err = v4_hal_uart_putc(0, 'A');
    CHECK(err == 0);

    int len;
    const char* tx = mock_hal_uart_get_tx(0, &len);
    CHECK(len == 1);
    CHECK(tx[0] == 'A');
  }

  SUBCASE("Receive character")
  {
    v4_hal_uart_init(0, 9600);
    mock_hal_uart_inject_rx(0, "Hello", 5);

    char c;
    v4_err err = v4_hal_uart_getc(0, &c);
    CHECK(err == 0);
    CHECK(c == 'H');

    err = v4_hal_uart_getc(0, &c);
    CHECK(err == 0);
    CHECK(c == 'e');
  }

  SUBCASE("UART buffer operations")
  {
    v4_hal_uart_init(1, 115200);

    const char* msg = "Test";
    v4_err err = v4_hal_uart_write(1, msg, 4);
    CHECK(err == 0);

    int len;
    const char* tx = mock_hal_uart_get_tx(1, &len);
    CHECK(len == 4);
    CHECK(strncmp(tx, "Test", 4) == 0);
  }
}

TEST_CASE("Mock HAL - Timer operations")
{
  mock_hal_reset();

  SUBCASE("Get milliseconds")
  {
    mock_hal_set_millis(1000);
    CHECK(v4_hal_millis() == 1000);
  }

  SUBCASE("Get microseconds")
  {
    mock_hal_set_micros(500000);
    CHECK(v4_hal_micros() == 500000);
  }

  SUBCASE("Delay milliseconds")
  {
    mock_hal_set_millis(0);
    v4_hal_delay_ms(100);
    CHECK(v4_hal_millis() == 100);
  }

  SUBCASE("Delay microseconds")
  {
    mock_hal_set_micros(0);
    v4_hal_delay_us(50);
    CHECK(v4_hal_micros() == 50);
  }
}

TEST_CASE("Mock HAL - System operations")
{
  SUBCASE("Get system info")
  {
    const char* info = v4_hal_system_info();
    CHECK(info != nullptr);
    CHECK(strlen(info) > 0);
  }

  SUBCASE("System reset")
  {
    v4_hal_gpio_init(5, V4_HAL_GPIO_MODE_OUTPUT);
    v4_hal_gpio_write(5, 1);

    v4_hal_system_reset();

    // After reset, GPIO should be cleared
    int value = mock_hal_gpio_get_value(5);
    CHECK(value == 0);
  }
}
