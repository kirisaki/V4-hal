#ifndef V4_HAL_UART_IMPL_HPP
#define V4_HAL_UART_IMPL_HPP

/**
 * @file uart_impl.hpp
 * @brief UART internal implementation using CRTP
 *
 * Provides platform-agnostic UART operations with handle-based management.
 * Uses CRTP for compile-time polymorphism.
 *
 * Platform requirements:
 * - static constexpr int max_uart_ports()
 * - static hal_handle_t uart_open_impl(int port, const hal_uart_config_t* config)
 * - static int uart_close_impl(hal_handle_t handle)
 * - static int uart_write_impl(hal_handle_t handle, const uint8_t* buf, size_t len)
 * - static int uart_read_impl(hal_handle_t handle, uint8_t* buf, size_t len)
 * - static int uart_available_impl(hal_handle_t handle)
 */

#include <cstdint>

#include "v4/hal_error.h"
#include "v4/hal_types.h"

namespace v4
{
namespace hal
{

/**
 * @brief UART handle structure (opaque pointer implementation)
 *
 * Internal structure for UART handle. Platforms can extend this
 * with platform_data field.
 */
struct UartHandle
{
  int port;            /**< UART port number */
  void* platform_data; /**< Platform-specific data */
};

/**
 * @brief UART base class with CRTP pattern
 *
 * Provides handle-based UART operations with parameter validation.
 * Platform implementations are called via static dispatch.
 *
 * @tparam Platform Platform implementation class
 */
template <typename Platform>
class UartBase
{
 public:
  /**
   * @brief Open UART port
   *
   * Validates port number and configuration, then delegates to platform.
   *
   * @param port   UART port number (platform-specific)
   * @param config Pointer to UART configuration
   * @return Opaque handle on success, nullptr on failure
   */
  static hal_handle_t open(int port, const hal_uart_config_t* config)
  {
    if (!config || port < 0 || port >= Platform::max_uart_ports())
    {
      return nullptr;
    }
    return Platform::uart_open_impl(port, config);
  }

  /**
   * @brief Close UART port
   *
   * @param handle UART handle from open()
   * @return HAL_OK on success, HAL_ERR_PARAM if handle invalid
   */
  static int close(hal_handle_t handle)
  {
    if (!handle)
      return HAL_ERR_PARAM;
    return Platform::uart_close_impl(handle);
  }

  /**
   * @brief Write data to UART
   *
   * @param handle UART handle
   * @param buf    Data buffer
   * @param len    Number of bytes to write
   * @return Number of bytes written, or negative error code
   */
  static int write(hal_handle_t handle, const uint8_t* buf, size_t len)
  {
    if (!handle || !buf)
      return HAL_ERR_PARAM;
    return Platform::uart_write_impl(handle, buf, len);
  }

  /**
   * @brief Read data from UART
   *
   * Non-blocking read. Returns immediately with available data.
   *
   * @param handle UART handle
   * @param buf    Destination buffer
   * @param len    Maximum bytes to read
   * @return Number of bytes read, or negative error code
   */
  static int read(hal_handle_t handle, uint8_t* buf, size_t len)
  {
    if (!handle || !buf)
      return HAL_ERR_PARAM;
    return Platform::uart_read_impl(handle, buf, len);
  }

  /**
   * @brief Get number of bytes available in receive buffer
   *
   * @param handle UART handle
   * @return Number of bytes available, or negative error code
   */
  static int available(hal_handle_t handle)
  {
    if (!handle)
      return HAL_ERR_PARAM;
    return Platform::uart_available_impl(handle);
  }
};

/**
 * @brief RAII wrapper for UART port
 *
 * Provides automatic resource management for UART ports.
 * Port is opened on construction and closed on destruction.
 *
 * Example:
 * @code
 * hal_uart_config_t cfg = {115200, 8, 1, 0};
 * Uart<PosixPlatform> uart(0, &cfg);
 * if (uart.is_open()) {
 *   uart.write((uint8_t*)"Hello", 5);
 * }
 * @endcode
 *
 * @tparam Platform Platform implementation class
 */
template <typename Platform>
class Uart
{
  hal_handle_t handle_;

 public:
  /**
   * @brief Open UART port
   *
   * @param port   UART port number
   * @param config UART configuration
   */
  Uart(int port, const hal_uart_config_t* config)
      : handle_(UartBase<Platform>::open(port, config))
  {
  }

  /**
   * @brief Close UART port on destruction
   */
  ~Uart()
  {
    if (handle_)
    {
      UartBase<Platform>::close(handle_);
    }
  }

  /**
   * @brief Check if UART is open
   *
   * @return true if open, false otherwise
   */
  bool is_open() const
  {
    return handle_ != nullptr;
  }

  /**
   * @brief Get raw handle
   *
   * @return UART handle (for advanced use)
   */
  hal_handle_t handle() const
  {
    return handle_;
  }

  /**
   * @brief Write data to UART
   *
   * @param buf Data buffer
   * @param len Number of bytes to write
   * @return Number of bytes written, or negative error code
   */
  int write(const uint8_t* buf, size_t len)
  {
    return UartBase<Platform>::write(handle_, buf, len);
  }

  /**
   * @brief Read data from UART
   *
   * @param buf Destination buffer
   * @param len Maximum bytes to read
   * @return Number of bytes read, or negative error code
   */
  int read(uint8_t* buf, size_t len)
  {
    return UartBase<Platform>::read(handle_, buf, len);
  }

  /**
   * @brief Get bytes available in receive buffer
   *
   * @return Number of bytes available, or negative error code
   */
  int available()
  {
    return UartBase<Platform>::available(handle_);
  }

  // Copy is prohibited (single ownership)
  Uart(const Uart&) = delete;
  Uart& operator=(const Uart&) = delete;

  // Move is allowed
  Uart(Uart&& other) noexcept : handle_(other.handle_)
  {
    other.handle_ = nullptr;
  }

  Uart& operator=(Uart&& other) noexcept
  {
    if (this != &other)
    {
      if (handle_)
      {
        UartBase<Platform>::close(handle_);
      }
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }
};

}  // namespace hal
}  // namespace v4

#endif  // V4_HAL_UART_IMPL_HPP
