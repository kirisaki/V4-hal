#ifndef V4_HAL_GPIO_IMPL_HPP
#define V4_HAL_GPIO_IMPL_HPP

/**
 * @file gpio_impl.hpp
 * @brief GPIO internal implementation using CRTP
 *
 * Provides platform-agnostic GPIO operations with zero-cost abstraction.
 * Uses Curiously Recurring Template Pattern (CRTP) for compile-time
 * polymorphism without virtual function overhead.
 *
 * Platform requirements:
 * - static constexpr int max_gpio_pins()
 * - static int gpio_mode_impl(int pin, hal_gpio_mode_t mode)
 * - static int gpio_write_impl(int pin, hal_gpio_value_t value)
 * - static int gpio_read_impl(int pin, hal_gpio_value_t* value)
 */

#include "v4/hal_error.h"
#include "v4/hal_types.h"

namespace v4
{
namespace hal
{

/**
 * @brief GPIO base class with CRTP pattern
 *
 * Provides common GPIO operations with parameter validation.
 * Platform-specific implementations are called via static dispatch.
 *
 * @tparam Platform Platform implementation class
 */
template <typename Platform>
class GpioBase
{
 public:
  /**
   * @brief Configure GPIO pin mode
   *
   * Validates pin number and delegates to platform implementation.
   *
   * @param pin  GPIO pin number
   * @param mode Pin mode (input, output, etc.)
   * @return HAL_OK on success, HAL_ERR_PARAM on invalid pin
   */
  static int mode(int pin, hal_gpio_mode_t mode)
  {
    if (pin < 0 || pin >= Platform::max_gpio_pins())
    {
      return HAL_ERR_PARAM;
    }
    return Platform::gpio_mode_impl(pin, mode);
  }

  /**
   * @brief Write value to GPIO pin
   *
   * @param pin   GPIO pin number
   * @param value HAL_GPIO_LOW or HAL_GPIO_HIGH
   * @return HAL_OK on success, HAL_ERR_PARAM on invalid pin
   */
  static int write(int pin, hal_gpio_value_t value)
  {
    if (pin < 0 || pin >= Platform::max_gpio_pins())
    {
      return HAL_ERR_PARAM;
    }
    return Platform::gpio_write_impl(pin, value);
  }

  /**
   * @brief Read value from GPIO pin
   *
   * @param pin   GPIO pin number
   * @param value Pointer to store read value
   * @return HAL_OK on success, HAL_ERR_PARAM on invalid arguments
   */
  static int read(int pin, hal_gpio_value_t* value)
  {
    if (!value || pin < 0 || pin >= Platform::max_gpio_pins())
    {
      return HAL_ERR_PARAM;
    }
    return Platform::gpio_read_impl(pin, value);
  }

  /**
   * @brief Toggle GPIO pin value
   *
   * Reads current pin state and writes the opposite value.
   * Useful for LED blinking and other periodic toggles.
   *
   * @param pin GPIO pin number
   * @return HAL_OK on success, negative error code on failure
   */
  static int toggle(int pin)
  {
    hal_gpio_value_t current;
    int ret = read(pin, &current);
    if (ret != HAL_OK)
      return ret;

    hal_gpio_value_t new_val = (current == HAL_GPIO_HIGH) ? HAL_GPIO_LOW : HAL_GPIO_HIGH;
    return write(pin, new_val);
  }
};

/**
 * @brief RAII wrapper for GPIO pin
 *
 * Provides automatic resource management and type-safe GPIO operations.
 * Pin is configured on construction and can be used throughout lifetime.
 *
 * Example:
 * @code
 * GpioPin<PosixPlatform> led(13, HAL_GPIO_OUTPUT);
 * led.write(HAL_GPIO_HIGH);
 * led.toggle();
 * @endcode
 *
 * @tparam Platform Platform implementation class
 */
template <typename Platform>
class GpioPin
{
  int pin_;

 public:
  /**
   * @brief Construct and configure GPIO pin
   *
   * @param pin  GPIO pin number
   * @param mode Pin mode
   */
  explicit GpioPin(int pin, hal_gpio_mode_t mode) : pin_(pin)
  {
    GpioBase<Platform>::mode(pin_, mode);
  }

  /**
   * @brief Write value to pin
   *
   * @param value HAL_GPIO_LOW or HAL_GPIO_HIGH
   * @return HAL_OK on success, negative error code on failure
   */
  int write(hal_gpio_value_t value)
  {
    return GpioBase<Platform>::write(pin_, value);
  }

  /**
   * @brief Read value from pin
   *
   * @param value Pointer to store read value
   * @return HAL_OK on success, negative error code on failure
   */
  int read(hal_gpio_value_t* value)
  {
    return GpioBase<Platform>::read(pin_, value);
  }

  /**
   * @brief Toggle pin value
   *
   * @return HAL_OK on success, negative error code on failure
   */
  int toggle()
  {
    return GpioBase<Platform>::toggle(pin_);
  }

  /**
   * @brief Get pin number
   *
   * @return GPIO pin number
   */
  int pin() const
  {
    return pin_;
  }

  // Copy is prohibited (single ownership)
  GpioPin(const GpioPin&) = delete;
  GpioPin& operator=(const GpioPin&) = delete;

  // Move is allowed
  GpioPin(GpioPin&& other) noexcept : pin_(other.pin_)
  {
    other.pin_ = -1;
  }

  GpioPin& operator=(GpioPin&& other) noexcept
  {
    if (this != &other)
    {
      pin_ = other.pin_;
      other.pin_ = -1;
    }
    return *this;
  }
};

}  // namespace hal
}  // namespace v4

#endif  // V4_HAL_GPIO_IMPL_HPP
