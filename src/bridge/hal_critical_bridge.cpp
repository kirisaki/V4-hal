/**
 * @file hal_critical_bridge.cpp
 * @brief C-language bridge for critical section API
 *
 * Provides C-compatible wrappers for C++ critical section implementation.
 */

#include "v4/hal.h"
#include "v4/hal_platform.hpp"
#include "v4/internal/critical_impl.hpp"

using Platform = HAL_PLATFORM_TYPE;
using Critical = v4::hal::CriticalImpl<Platform>;

extern "C"
{
  void hal_critical_enter(void)
  {
    Critical::critical_enter();
  }

  void hal_critical_exit(void)
  {
    Critical::critical_exit();
  }

}  // extern "C"
