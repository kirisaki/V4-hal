#include "v4/hal.h"

/**
 * @file hal_gpio_bridge.cpp
 * @brief extern "C" bridge for GPIO operations
 *
 * Bridges between C API (hal.h) and C++17 internal implementation.
 * Platform selection is done at compile time via preprocessor macros.
 */

#include "../internal/gpio_impl.hpp"

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

using GpioImpl = v4::hal::GpioBase<Platform>;

/* ========================================================================= */
/* extern "C" GPIO API Implementation                                        */
/* ========================================================================= */

extern "C"
{
  int hal_gpio_mode(int pin, hal_gpio_mode_t mode)
  {
    return GpioImpl::mode(pin, mode);
  }

  int hal_gpio_write(int pin, hal_gpio_value_t value)
  {
    return GpioImpl::write(pin, value);
  }

  int hal_gpio_read(int pin, hal_gpio_value_t* value)
  {
    return GpioImpl::read(pin, value);
  }

  int hal_gpio_toggle(int pin)
  {
    return GpioImpl::toggle(pin);
  }

  // GPIO interrupt stubs (not yet implemented)
  int hal_gpio_irq_attach(int pin, hal_gpio_irq_edge_t edge,
                          hal_gpio_irq_handler_t handler, void* user_data)
  {
    (void)pin;
    (void)edge;
    (void)handler;
    (void)user_data;
    return HAL_ERR_NOTSUP;  // Not supported yet
  }

  int hal_gpio_irq_detach(int pin)
  {
    (void)pin;
    return HAL_ERR_NOTSUP;
  }

  int hal_gpio_irq_enable(int pin)
  {
    (void)pin;
    return HAL_ERR_NOTSUP;
  }

  int hal_gpio_irq_disable(int pin)
  {
    (void)pin;
    return HAL_ERR_NOTSUP;
  }

}  // extern "C"
