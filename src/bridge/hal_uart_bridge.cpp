#include "v4/hal.h"

/**
 * @file hal_uart_bridge.cpp
 * @brief extern "C" bridge for UART operations
 *
 * Bridges between C API (hal.h) and C++17 internal implementation.
 * Platform selection is done at compile time via preprocessor macros.
 */

#include "../internal/uart_impl.hpp"

// Platform selection (compile-time)
#ifdef HAL_PLATFORM_POSIX
#include "../../ports/posix/platform_posix.hpp"
using Platform = v4::hal::PosixPlatform;
#elif defined(HAL_PLATFORM_ESP32)
#include "../../ports/esp32/platform_esp32.hpp"
using Platform = v4::hal::Esp32Platform;
#elif defined(HAL_PLATFORM_CH32V203)
#include "../../ports/ch32v203/platform_ch32v203.hpp"
using Platform = v4::hal::Ch32v203Platform;
#else
#error \
    "No HAL platform defined. Define HAL_PLATFORM_POSIX, HAL_PLATFORM_ESP32, or HAL_PLATFORM_CH32V203."
#endif

using UartImpl = v4::hal::UartBase<Platform>;

/* ========================================================================= */
/* extern "C" UART API Implementation                                        */
/* ========================================================================= */

extern "C"
{
  hal_handle_t hal_uart_open(int port, const hal_uart_config_t* config)
  {
    return UartImpl::open(port, config);
  }

  int hal_uart_close(hal_handle_t handle)
  {
    return UartImpl::close(handle);
  }

  int hal_uart_write(hal_handle_t handle, const uint8_t* buf, size_t len)
  {
    return UartImpl::write(handle, buf, len);
  }

  int hal_uart_read(hal_handle_t handle, uint8_t* buf, size_t len)
  {
    return UartImpl::read(handle, buf, len);
  }

  int hal_uart_available(hal_handle_t handle)
  {
    return UartImpl::available(handle);
  }

}  // extern "C"
