#ifndef V4_HAL_TIMER_IMPL_HPP
#define V4_HAL_TIMER_IMPL_HPP

/**
 * @file timer_impl.hpp
 * @brief Timer internal implementation using CRTP
 *
 * Provides platform-agnostic timer operations for time measurement
 * and delays. Uses CRTP for compile-time polymorphism.
 *
 * Platform requirements:
 * - static uint32_t millis_impl()
 * - static uint64_t micros_impl()
 * - static void delay_ms_impl(uint32_t ms)
 * - static void delay_us_impl(uint32_t us)
 */

#include <cstdint>

namespace v4
{
namespace hal
{

/**
 * @brief Timer base class with CRTP pattern
 *
 * Provides time measurement and delay operations.
 * Platform implementations are called via static dispatch.
 *
 * @tparam Platform Platform implementation class
 */
template <typename Platform>
class TimerBase
{
 public:
  /**
   * @brief Get milliseconds since system startup
   *
   * Returns the number of milliseconds elapsed since the system
   * started. This value wraps around after approximately 49 days
   * (2^32 milliseconds).
   *
   * @return Milliseconds since startup
   */
  static uint32_t millis()
  {
    return Platform::millis_impl();
  }

  /**
   * @brief Get microseconds since system startup
   *
   * Returns the number of microseconds elapsed since the system
   * started. Uses 64-bit counter to avoid wrap-around for
   * approximately 584,942 years.
   *
   * @return Microseconds since startup
   */
  static uint64_t micros()
  {
    return Platform::micros_impl();
  }

  /**
   * @brief Blocking delay in milliseconds
   *
   * Blocks execution for the specified number of milliseconds.
   * May use busy-wait or low-power sleep depending on platform.
   *
   * @param ms Milliseconds to delay
   */
  static void delay_ms(uint32_t ms)
  {
    Platform::delay_ms_impl(ms);
  }

  /**
   * @brief Blocking delay in microseconds
   *
   * Blocks execution for the specified number of microseconds.
   * Typically uses busy-wait for accurate short delays.
   *
   * @param us Microseconds to delay
   */
  static void delay_us(uint32_t us)
  {
    Platform::delay_us_impl(us);
  }

  /**
   * @brief Calculate elapsed time in milliseconds
   *
   * Helper function to calculate elapsed time accounting for
   * potential wrap-around of the 32-bit millisecond counter.
   *
   * @param start_ms Start time from millis()
   * @return Elapsed milliseconds (handles wrap-around)
   */
  static uint32_t elapsed_ms(uint32_t start_ms)
  {
    uint32_t now = millis();
    // Handle wrap-around
    return (now >= start_ms) ? (now - start_ms) : (0xFFFFFFFF - start_ms + now + 1);
  }

  /**
   * @brief Calculate elapsed time in microseconds
   *
   * Helper function to calculate elapsed time. No wrap-around
   * handling needed due to 64-bit counter.
   *
   * @param start_us Start time from micros()
   * @return Elapsed microseconds
   */
  static uint64_t elapsed_us(uint64_t start_us)
  {
    return micros() - start_us;
  }
};

}  // namespace hal
}  // namespace v4

#endif  // V4_HAL_TIMER_IMPL_HPP
