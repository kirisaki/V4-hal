#include "v4/hal_capabilities.h"

/**
 * @file hal_capabilities.cpp
 * @brief Platform capability system implementation
 *
 * Provides default capability structure and dispatches to
 * platform-specific implementations via weak symbols.
 */

/**
 * @brief Platform-specific capability provider (weak symbol)
 *
 * Each platform implementation should provide this function.
 * If not provided, returns default (zero) capabilities.
 *
 * @return Pointer to platform capability structure
 */
__attribute__((weak)) const hal_capabilities_t* hal_platform_capabilities(void)
{
  static const hal_capabilities_t default_caps = {
      0,  // gpio_count
      0,  // uart_count
      0,  // spi_count
      0,  // i2c_count
      0,  // has_adc
      0,  // has_dac
      0,  // has_pwm
      0,  // has_rtc
      0,  // has_dma
      0,  // reserved
  };
  return &default_caps;
}

extern "C" const hal_capabilities_t* hal_get_capabilities(void)
{
  return hal_platform_capabilities();
}
