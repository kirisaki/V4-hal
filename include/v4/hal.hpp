#pragma once

/**
 * @file hal.hpp
 * @brief C++ wrapper for V4 HAL API
 *
 * Provides RAII-based resource management and exception-based error handling
 * for the V4 Hardware Abstraction Layer.
 *
 * Features:
 * - RAII wrappers for HAL resources (Uart, GpioPin, HalSystem)
 * - Exception-based error handling (Error class)
 * - Type-safe interfaces
 * - Move semantics for resource handles
 */

#include "v4/hal.h"
#include "v4/hal_error.h"
#include "v4/hal_types.h"

#include <stdexcept>
#include <string>

namespace v4
{
namespace hal
{

  /* ========================================================================= */
  /* Error handling                                                            */
  /* ========================================================================= */

  /**
   * @brief HAL error exception class
   *
   * Thrown when a HAL function returns an error code.
   * Derives from std::runtime_error for standard exception handling.
   */
  class Error : public std::runtime_error
  {
  public:
    /**
     * @brief Construct error from HAL error code
     * @param code HAL error code (negative value)
     */
    explicit Error(int code)
        : std::runtime_error(hal_strerror(code)), code_(code)
    {
    }

    /**
     * @brief Get the HAL error code
     * @return Error code (negative value)
     */
    int code() const
    {
      return code_;
    }

  private:
    int code_;
  };

  /**
   * @brief Check HAL result and throw on error
   * @param result Return value from HAL function
   * @throws Error if result is negative
   */
  inline void check(int result)
  {
    if (result < 0)
    {
      throw Error(result);
    }
  }

  /* ========================================================================= */
  /* HAL System Management                                                     */
  /* ========================================================================= */

  /**
   * @brief RAII wrapper for HAL system initialization
   *
   * Automatically calls hal_init() on construction and hal_deinit() on
   * destruction. Non-copyable but provides reset() method.
   *
   * Example:
   * @code
   * {
   *   v4::hal::HalSystem hal;  // Initializes HAL
   *   // Use HAL functions...
   * }  // Automatically deinitializes HAL
   * @endcode
   */
  class HalSystem
  {
  public:
    /**
     * @brief Initialize HAL system
     * @throws Error if initialization fails
     */
    HalSystem()
    {
      int ret = hal_init();
      if (ret < 0)
        throw Error(ret);
    }

    /**
     * @brief Deinitialize HAL system
     */
    ~HalSystem()
    {
      hal_deinit();
    }

    // Non-copyable
    HalSystem(const HalSystem&) = delete;
    HalSystem& operator=(const HalSystem&) = delete;

    /**
     * @brief Reset HAL system to initial state
     * @throws Error if reset fails
     */
    void reset()
    {
      check(hal_reset());
    }
  };

  /* ========================================================================= */
  /* GPIO                                                                      */
  /* ========================================================================= */

  /**
   * @brief RAII wrapper for GPIO pin
   *
   * Manages a GPIO pin with type-safe mode configuration and value access.
   *
   * Example:
   * @code
   * v4::hal::GpioPin led(13, HAL_GPIO_OUTPUT);
   * led.write(HAL_GPIO_HIGH);
   * led.toggle();
   * @endcode
   */
  class GpioPin
  {
  public:
    /**
     * @brief Configure GPIO pin
     * @param pin Pin number (platform-specific)
     * @param mode Pin mode (input, output, etc.)
     * @throws Error if configuration fails
     */
    GpioPin(int pin, hal_gpio_mode_t mode) : pin_(pin)
    {
      check(hal_gpio_mode(pin_, mode));
    }

    /**
     * @brief Write value to output pin
     * @param value HAL_GPIO_LOW or HAL_GPIO_HIGH
     * @throws Error if write fails
     */
    void write(hal_gpio_value_t value)
    {
      check(hal_gpio_write(pin_, value));
    }

    /**
     * @brief Read value from pin
     * @return Current pin value (HAL_GPIO_LOW or HAL_GPIO_HIGH)
     * @throws Error if read fails
     */
    hal_gpio_value_t read()
    {
      hal_gpio_value_t value;
      check(hal_gpio_read(pin_, &value));
      return value;
    }

    /**
     * @brief Toggle output pin value
     * @throws Error if toggle fails
     */
    void toggle()
    {
      check(hal_gpio_toggle(pin_));
    }

    /**
     * @brief Get pin number
     * @return Pin number
     */
    int pin() const
    {
      return pin_;
    }

  private:
    int pin_;
  };

  /* ========================================================================= */
  /* UART                                                                      */
  /* ========================================================================= */

  /**
   * @brief RAII wrapper for UART port
   *
   * Automatically opens UART on construction and closes on destruction.
   * Non-copyable but movable.
   *
   * Example:
   * @code
   * hal_uart_config_t config = {115200, 8, 1, 0};
   * v4::hal::Uart uart(0, config);
   * uint8_t data[] = "Hello";
   * uart.write(data, 5);
   * @endcode
   */
  class Uart
  {
  public:
    /**
     * @brief Open UART port
     * @param port UART port number (platform-specific, typically 0-3)
     * @param config UART configuration (baudrate, data bits, etc.)
     * @throws Error if open fails
     */
    Uart(int port, const hal_uart_config_t& config)
    {
      handle_ = hal_uart_open(port, &config);
      if (!handle_)
        throw Error(HAL_ERR_NODEV);
    }

    /**
     * @brief Close UART port
     */
    ~Uart()
    {
      if (handle_)
      {
        hal_uart_close(handle_);
      }
    }

    // Non-copyable
    Uart(const Uart&) = delete;
    Uart& operator=(const Uart&) = delete;

    // Movable
    Uart(Uart&& other) noexcept : handle_(other.handle_)
    {
      other.handle_ = nullptr;
    }

    Uart& operator=(Uart&& other) noexcept
    {
      if (this != &other)
      {
        if (handle_)
          hal_uart_close(handle_);
        handle_ = other.handle_;
        other.handle_ = nullptr;
      }
      return *this;
    }

    /**
     * @brief Write data to UART
     * @param buf Data buffer
     * @param len Number of bytes to write
     * @return Number of bytes written
     * @throws Error if write fails
     */
    int write(const uint8_t* buf, size_t len)
    {
      int ret = hal_uart_write(handle_, buf, len);
      if (ret < 0)
        throw Error(ret);
      return ret;
    }

    /**
     * @brief Read data from UART
     * @param buf Destination buffer
     * @param len Maximum bytes to read
     * @return Number of bytes read
     * @throws Error if read fails
     */
    int read(uint8_t* buf, size_t len)
    {
      int ret = hal_uart_read(handle_, buf, len);
      if (ret < 0)
        throw Error(ret);
      return ret;
    }

    /**
     * @brief Get number of bytes available in receive buffer
     * @return Number of bytes available
     * @throws Error if query fails
     */
    int available()
    {
      int ret = hal_uart_available(handle_);
      if (ret < 0)
        throw Error(ret);
      return ret;
    }

  private:
    hal_handle_t handle_;
  };

  /* ========================================================================= */
  /* Timer utilities                                                           */
  /* ========================================================================= */

  /**
   * @brief Get milliseconds since system startup
   * @return Milliseconds (wraps after ~49 days)
   */
  inline uint32_t millis()
  {
    return hal_millis();
  }

  /**
   * @brief Get microseconds since system startup
   * @return Microseconds (wraps after ~584,942 years)
   */
  inline uint64_t micros()
  {
    return hal_micros();
  }

  /**
   * @brief Blocking delay in milliseconds
   * @param ms Milliseconds to delay
   */
  inline void delay_ms(uint32_t ms)
  {
    hal_delay_ms(ms);
  }

  /**
   * @brief Blocking delay in microseconds
   * @param us Microseconds to delay
   */
  inline void delay_us(uint32_t us)
  {
    hal_delay_us(us);
  }

  /* ========================================================================= */
  /* Console I/O utilities                                                     */
  /* ========================================================================= */

  /**
   * @brief Write data to console output
   * @param buf Data buffer
   * @param len Number of bytes to write
   * @return Number of bytes written
   * @throws Error if write fails
   */
  inline int console_write(const uint8_t* buf, size_t len)
  {
    int ret = hal_console_write(buf, len);
    if (ret < 0)
      throw Error(ret);
    return ret;
  }

  /**
   * @brief Read data from console input (blocking)
   * @param buf Destination buffer
   * @param len Maximum bytes to read
   * @return Number of bytes read
   * @throws Error if read fails
   */
  inline int console_read(uint8_t* buf, size_t len)
  {
    int ret = hal_console_read(buf, len);
    if (ret < 0)
      throw Error(ret);
    return ret;
  }

}  // namespace hal
}  // namespace v4
