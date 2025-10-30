#include "v4/hal.h"

/**
 * @file hal_timer_bridge.cpp
 * @brief extern "C" bridge for Timer operations
 *
 * Bridges between C API (hal.h) and C++17 internal implementation.
 * Platform selection is done at compile time via preprocessor macros.
 */

#include "../internal/timer_impl.hpp"

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

using TimerImpl = v4::hal::TimerBase<Platform>;

/* ========================================================================= */
/* extern "C" Timer API Implementation                                       */
/* ========================================================================= */

extern "C"
{
  uint32_t hal_millis(void)
  {
    return TimerImpl::millis();
  }

  uint64_t hal_micros(void)
  {
    return TimerImpl::micros();
  }

  void hal_delay_ms(uint32_t ms)
  {
    TimerImpl::delay_ms(ms);
  }

  void hal_delay_us(uint32_t us)
  {
    TimerImpl::delay_us(us);
  }

}  // extern "C"
