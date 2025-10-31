#include "v4/hal.h"

/**
 * @file hal_console_bridge.cpp
 * @brief extern "C" bridge for Console I/O operations
 *
 * Bridges between C API (hal.h) and C++17 internal implementation.
 * Platform selection is done at compile time via preprocessor macros.
 */

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

/* ========================================================================= */
/* extern "C" Console I/O API Implementation                                 */
/* ========================================================================= */

extern "C"
{
  int hal_console_write(const uint8_t* buf, size_t len)
  {
    return Platform::console_write_impl(buf, len);
  }

  int hal_console_read(uint8_t* buf, size_t len)
  {
    return Platform::console_read_impl(buf, len);
  }

}  // extern "C"
