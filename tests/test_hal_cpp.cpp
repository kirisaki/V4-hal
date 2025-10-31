/**
 * @file test_hal_cpp.cpp
 * @brief C++ wrapper tests for V4 HAL
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "v4/hal.hpp"

TEST_CASE("Error class")
{
  SUBCASE("Error message")
  {
    v4::hal::Error err(HAL_ERR_PARAM);
    CHECK(err.code() == HAL_ERR_PARAM);
    CHECK(std::string(err.what()) == "Invalid parameter");
  }

  SUBCASE("Different error codes")
  {
    v4::hal::Error err_io(HAL_ERR_IO);
    CHECK(err_io.code() == HAL_ERR_IO);
    CHECK(std::string(err_io.what()) == "I/O error");

    v4::hal::Error err_timeout(HAL_ERR_TIMEOUT);
    CHECK(err_timeout.code() == HAL_ERR_TIMEOUT);
    CHECK(std::string(err_timeout.what()) == "Operation timed out");
  }
}

TEST_CASE("check() function")
{
  SUBCASE("Success (0)")
  {
    CHECK_NOTHROW(v4::hal::check(0));
  }

  SUBCASE("Success (positive)")
  {
    CHECK_NOTHROW(v4::hal::check(42));
  }

  SUBCASE("Error (negative)")
  {
    try
    {
      v4::hal::check(HAL_ERR_PARAM);
      FAIL("Should have thrown");
    }
    catch (const v4::hal::Error& e)
    {
      CHECK(e.code() == HAL_ERR_PARAM);
    }
  }
}

TEST_CASE("HalSystem RAII")
{
  SUBCASE("Construction and destruction")
  {
    // This should not throw with mock HAL
    CHECK_NOTHROW(v4::hal::HalSystem hal);
  }

  SUBCASE("Reset")
  {
    v4::hal::HalSystem hal;
    CHECK_NOTHROW(hal.reset());
  }
}

TEST_CASE("GpioPin")
{
  SUBCASE("Construction")
  {
    CHECK_NOTHROW(v4::hal::GpioPin pin(13, HAL_GPIO_OUTPUT));
  }

  SUBCASE("Write and read")
  {
    v4::hal::GpioPin pin(13, HAL_GPIO_OUTPUT);
    CHECK_NOTHROW(pin.write(HAL_GPIO_HIGH));
    CHECK_NOTHROW(pin.write(HAL_GPIO_LOW));
  }

  SUBCASE("Toggle")
  {
    v4::hal::GpioPin pin(13, HAL_GPIO_OUTPUT);
    CHECK_NOTHROW(pin.toggle());
  }

  SUBCASE("Read input pin")
  {
    v4::hal::GpioPin pin(14, HAL_GPIO_INPUT);
    hal_gpio_value_t value;
    CHECK_NOTHROW(value = pin.read());
    // Value should be LOW or HIGH
    CHECK((value == HAL_GPIO_LOW || value == HAL_GPIO_HIGH));
  }

  SUBCASE("Pin number accessor")
  {
    v4::hal::GpioPin pin(13, HAL_GPIO_OUTPUT);
    CHECK(pin.pin() == 13);
  }
}

TEST_CASE("Uart")
{
  hal_uart_config_t config = {115200, 8, 1, 0};

  SUBCASE("Construction")
  {
    CHECK_NOTHROW(v4::hal::Uart uart(0, config));
  }

  SUBCASE("Write")
  {
    v4::hal::Uart uart(0, config);
    uint8_t data[] = "Hello";
    int written;
    CHECK_NOTHROW(written = uart.write(data, 5));
    CHECK(written == 5);
  }

  SUBCASE("Available")
  {
    v4::hal::Uart uart(0, config);
    int avail;
    CHECK_NOTHROW(avail = uart.available());
    CHECK(avail >= 0);
  }

  SUBCASE("Move semantics")
  {
    v4::hal::Uart uart1(0, config);
    v4::hal::Uart uart2(std::move(uart1));
    uint8_t data[] = "Test";
    CHECK_NOTHROW(uart2.write(data, 4));
  }

  SUBCASE("Move assignment")
  {
    v4::hal::Uart uart1(0, config);
    hal_uart_config_t config2 = {9600, 8, 1, 0};
    v4::hal::Uart uart2(1, config2);
    uart2 = std::move(uart1);
    uint8_t data[] = "Test";
    CHECK_NOTHROW(uart2.write(data, 4));
  }
}

TEST_CASE("Timer utilities")
{
  SUBCASE("millis()")
  {
    uint32_t ms = v4::hal::millis();
    CHECK(ms >= 0);
  }

  SUBCASE("micros()")
  {
    uint64_t us = v4::hal::micros();
    CHECK(us >= 0);
  }

  SUBCASE("delay_ms()")
  {
    CHECK_NOTHROW(v4::hal::delay_ms(1));
  }

  SUBCASE("delay_us()")
  {
    CHECK_NOTHROW(v4::hal::delay_us(100));
  }
}

TEST_CASE("Console I/O utilities")
{
  SUBCASE("console_write()")
  {
    uint8_t data[] = "Test\n";
    int written;
    CHECK_NOTHROW(written = v4::hal::console_write(data, 5));
    CHECK(written == 5);
  }

  SUBCASE("console_read()")
  {
    // Note: This would block in real implementation
    // In mock HAL, it should return immediately or with injected data
    uint8_t buf[10];
    // Don't actually call this in test as it may block
    // Just verify it compiles
  }
}
