#ifndef V4_HAL_PLATFORM_ESP32_HPP
#define V4_HAL_PLATFORM_ESP32_HPP

/**
 * @file platform_esp32.hpp
 * @brief ESP32 platform implementation for V4 HAL
 *
 * Provides HAL implementation for ESP32/ESP32-C6 using ESP-IDF.
 * Supports GPIO, UART, Timer, and Console I/O using ESP32 peripherals.
 */

#include <cstdint>

#include "v4/hal_types.h"

namespace v4
{
namespace hal
{

/**
 * @brief ESP32 platform implementation
 *
 * Provides CRTP implementation for GPIO, UART, and Timer operations.
 * Uses ESP-IDF APIs for hardware access.
 *
 * Compatible with ESP32, ESP32-S3, ESP32-C3, ESP32-C6, etc.
 */
struct Esp32Platform
{
  /* ======================================================================= */
  /* Platform Capabilities                                                   */
  /* ======================================================================= */

  /**
   * @brief Maximum number of GPIO pins
   *
   * ESP32 variants support up to 48 GPIO pins (varies by model).
   * ESP32-C6 has 30 GPIO pins (GPIO0-GPIO30).
   */
  static constexpr int max_gpio_pins()
  {
    return 48;
  }

  /**
   * @brief Maximum number of UART ports
   *
   * ESP32 supports up to 3 UART ports (UART0, UART1, UART2).
   */
  static constexpr int max_uart_ports()
  {
    return 3;
  }

  /* ======================================================================= */
  /* GPIO Implementation                                                     */
  /* ======================================================================= */

  static int gpio_mode_impl(int pin, hal_gpio_mode_t mode);
  static int gpio_write_impl(int pin, hal_gpio_value_t value);
  static int gpio_read_impl(int pin, hal_gpio_value_t* value);

  /* ======================================================================= */
  /* UART Implementation                                                     */
  /* ======================================================================= */

  static hal_handle_t uart_open_impl(int port, const hal_uart_config_t* config);
  static int uart_close_impl(hal_handle_t handle);
  static int uart_write_impl(hal_handle_t handle, const uint8_t* buf, size_t len);
  static int uart_read_impl(hal_handle_t handle, uint8_t* buf, size_t len);
  static int uart_available_impl(hal_handle_t handle);

  /* ======================================================================= */
  /* Timer Implementation                                                    */
  /* ======================================================================= */

  static uint32_t millis_impl();
  static uint64_t micros_impl();
  static void delay_ms_impl(uint32_t ms);
  static void delay_us_impl(uint32_t us);

  /* ======================================================================= */
  /* Console I/O Implementation                                              */
  /* ======================================================================= */

  static int console_write_impl(const uint8_t* buf, size_t len);
  static int console_read_impl(uint8_t* buf, size_t len);
};

}  // namespace hal
}  // namespace v4

#endif  // V4_HAL_PLATFORM_ESP32_HPP
