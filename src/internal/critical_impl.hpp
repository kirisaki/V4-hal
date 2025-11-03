#ifndef V4_HAL_INTERNAL_CRITICAL_IMPL_HPP
#define V4_HAL_INTERNAL_CRITICAL_IMPL_HPP

/**
 * @file critical_impl.hpp
 * @brief Critical section (interrupt control) implementation
 *
 * Provides platform-agnostic critical section primitives using CRTP pattern.
 * Platforms must implement:
 * - void critical_enter_impl()
 * - void critical_exit_impl()
 */

#include "v4/hal_platform.hpp"

namespace v4
{
namespace hal
{

/**
 * @brief Critical section implementation (CRTP base)
 *
 * Provides interrupt disable/enable primitives for thread-safe access
 * to shared resources.
 *
 * @tparam Platform Platform-specific implementation class
 */
template <typename Platform>
class CriticalImpl
{
 public:
  /**
   * @brief Enter critical section (disable interrupts)
   *
   * Disables interrupts to create a critical section.
   * Must support nesting - each enter must be paired with an exit.
   */
  static void critical_enter()
  {
    Platform::critical_enter_impl();
  }

  /**
   * @brief Exit critical section (enable interrupts)
   *
   * Re-enables interrupts after leaving critical section.
   * Must be paired with critical_enter().
   */
  static void critical_exit()
  {
    Platform::critical_exit_impl();
  }

 protected:
  ~CriticalImpl() = default;
};

}  // namespace hal
}  // namespace v4

#endif  // V4_HAL_INTERNAL_CRITICAL_IMPL_HPP
