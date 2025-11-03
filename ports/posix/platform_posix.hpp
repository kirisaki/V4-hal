#ifndef V4_HAL_PLATFORM_POSIX_HPP
#define V4_HAL_PLATFORM_POSIX_HPP

/**
 * @file platform_posix.hpp
 * @brief POSIX platform implementation for V4 HAL
 *
 * Provides HAL implementation for POSIX systems (Linux, macOS, BSD).
 * Uses simulation for GPIO (bitmap) and actual POSIX APIs for UART/Timer.
 */

#include <cstdint>

#include "v4/hal_types.h"

namespace v4
{
namespace hal
{

/**
 * @brief POSIX platform implementation
 *
 * Provides CRTP implementation for GPIO, UART, and Timer operations.
 * GPIO is simulated using bitmaps. UART uses stdin/stdout.
 * Timer uses clock_gettime(CLOCK_MONOTONIC).
 */
struct PosixPlatform
{
  /* ======================================================================= */
  /* Platform Capabilities                                                   */
  /* ======================================================================= */

  /**
   * @brief Maximum number of GPIO pins
   *
   * POSIX simulation supports 32 GPIO pins.
   */
  static constexpr int max_gpio_pins()
  {
    return 32;
  }

  /**
   * @brief Maximum number of UART ports
   *
   * POSIX simulation supports 4 UART ports.
   */
  static constexpr int max_uart_ports()
  {
    return 4;
  }

  /* ======================================================================= */
  /* GPIO Implementation                                                     */
  /* ======================================================================= */

  /**
   * @brief Configure GPIO pin mode
   *
   * Simulates GPIO mode configuration using internal state.
   *
   * @param pin  GPIO pin number
   * @param mode Pin mode
   * @return HAL_OK on success
   */
  static int gpio_mode_impl(int pin, hal_gpio_mode_t mode);

  /**
   * @brief Write value to GPIO pin
   *
   * Simulates GPIO write using bitmap.
   *
   * @param pin   GPIO pin number
   * @param value Pin value (LOW/HIGH)
   * @return HAL_OK on success, HAL_ERR_PARAM if not configured as output
   */
  static int gpio_write_impl(int pin, hal_gpio_value_t value);

  /**
   * @brief Read value from GPIO pin
   *
   * Reads simulated GPIO state from bitmap.
   *
   * @param pin   GPIO pin number
   * @param value Pointer to store read value
   * @return HAL_OK on success
   */
  static int gpio_read_impl(int pin, hal_gpio_value_t* value);

  /* ======================================================================= */
  /* UART Implementation                                                     */
  /* ======================================================================= */

  /**
   * @brief Open UART port
   *
   * Simulates UART using FILE* (stdout for port 0).
   *
   * @param port   UART port number
   * @param config UART configuration (ignored in simulation)
   * @return Opaque handle on success, nullptr on failure
   */
  static hal_handle_t uart_open_impl(int port, const hal_uart_config_t* config);

  /**
   * @brief Close UART port
   *
   * @param handle UART handle
   * @return HAL_OK on success
   */
  static int uart_close_impl(hal_handle_t handle);

  /**
   * @brief Write data to UART
   *
   * @param handle UART handle
   * @param buf    Data buffer
   * @param len    Number of bytes to write
   * @return Number of bytes written
   */
  static int uart_write_impl(hal_handle_t handle, const uint8_t* buf, size_t len);

  /**
   * @brief Read data from UART
   *
   * Non-blocking read (simplified implementation).
   *
   * @param handle UART handle
   * @param buf    Destination buffer
   * @param len    Maximum bytes to read
   * @return Number of bytes read (0 in simple implementation)
   */
  static int uart_read_impl(hal_handle_t handle, uint8_t* buf, size_t len);

  /**
   * @brief Get bytes available in UART receive buffer
   *
   * @param handle UART handle
   * @return 0 (simplified implementation)
   */
  static int uart_available_impl(hal_handle_t handle);

  /* ======================================================================= */
  /* Timer Implementation                                                    */
  /* ======================================================================= */

  /**
   * @brief Get milliseconds since startup
   *
   * Uses clock_gettime(CLOCK_MONOTONIC).
   *
   * @return Milliseconds since startup
   */
  static uint32_t millis_impl();

  /**
   * @brief Get microseconds since startup
   *
   * Uses clock_gettime(CLOCK_MONOTONIC).
   *
   * @return Microseconds since startup
   */
  static uint64_t micros_impl();

  /**
   * @brief Blocking delay in milliseconds
   *
   * Uses usleep().
   *
   * @param ms Milliseconds to delay
   */
  static void delay_ms_impl(uint32_t ms);

  /**
   * @brief Blocking delay in microseconds
   *
   * Uses usleep().
   *
   * @param us Microseconds to delay
   */
  static void delay_us_impl(uint32_t us);

  /* ======================================================================= */
  /* Console I/O Implementation                                              */
  /* ======================================================================= */

  /**
   * @brief Write data to console output
   *
   * Uses write(STDOUT_FILENO).
   *
   * @param buf Data buffer
   * @param len Number of bytes to write
   * @return Number of bytes written
   */
  static int console_write_impl(const uint8_t* buf, size_t len);

  /**
   * @brief Read data from console input
   *
   * Uses read(STDIN_FILENO). Blocking read.
   *
   * @param buf Destination buffer
   * @param len Maximum bytes to read
   * @return Number of bytes read
   */
  static int console_read_impl(uint8_t* buf, size_t len);

  /* ======================================================================= */
  /* Interrupt Control Implementation                                        */
  /* ======================================================================= */

  /**
   * @brief Enter critical section
   *
   * Uses pthread_mutex for thread safety on POSIX systems.
   */
  static void critical_enter_impl();

  /**
   * @brief Exit critical section
   *
   * Releases pthread_mutex.
   */
  static void critical_exit_impl();
};

}  // namespace hal
}  // namespace v4

#endif  // V4_HAL_PLATFORM_POSIX_HPP
