#include "platform_posix.hpp"

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "v4/hal_capabilities.h"
#include "v4/hal_error.h"

namespace v4
{
namespace hal
{

/* ========================================================================= */
/* GPIO Simulation State                                                     */
/* ========================================================================= */

static uint32_t gpio_states = 0;  // Pin values (0 or 1)
static uint32_t gpio_modes = 0;   // Pin modes (0=input, 1=output)

/* ========================================================================= */
/* GPIO Implementation                                                       */
/* ========================================================================= */

int PosixPlatform::gpio_mode_impl(int pin, hal_gpio_mode_t mode)
{
  // Simulate mode configuration by setting bit in gpio_modes
  if (mode == HAL_GPIO_OUTPUT || mode == HAL_GPIO_OUTPUT_OD)
  {
    gpio_modes |= (1u << pin);
  }
  else
  {
    gpio_modes &= ~(1u << pin);
  }
  return HAL_OK;
}

int PosixPlatform::gpio_write_impl(int pin, hal_gpio_value_t value)
{
  // Check if pin is configured as output
  if (!(gpio_modes & (1u << pin)))
  {
    return HAL_ERR_PARAM;  // Pin not configured as output
  }

  // Update pin state
  if (value == HAL_GPIO_HIGH)
  {
    gpio_states |= (1u << pin);
  }
  else
  {
    gpio_states &= ~(1u << pin);
  }
  return HAL_OK;
}

int PosixPlatform::gpio_read_impl(int pin, hal_gpio_value_t* value)
{
  *value = (gpio_states & (1u << pin)) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
  return HAL_OK;
}

/* ========================================================================= */
/* UART Simulation                                                           */
/* ========================================================================= */

struct UartHandleData
{
  int port;
  FILE* fp;
};

hal_handle_t PosixPlatform::uart_open_impl(int port, const hal_uart_config_t* config)
{
  (void)config;  // Unused in simulation

  auto* handle = new UartHandleData{port, nullptr};

  // Port 0 uses stdout for simulation
  if (port == 0)
  {
    handle->fp = stdout;
  }

  return static_cast<hal_handle_t>(handle);
}

int PosixPlatform::uart_close_impl(hal_handle_t handle)
{
  auto* h = static_cast<UartHandleData*>(handle);
  delete h;
  return HAL_OK;
}

int PosixPlatform::uart_write_impl(hal_handle_t handle, const uint8_t* buf, size_t len)
{
  auto* h = static_cast<UartHandleData*>(handle);
  if (h->fp)
  {
    size_t written = fwrite(buf, 1, len, h->fp);
    fflush(h->fp);
    return static_cast<int>(written);
  }
  return 0;
}

int PosixPlatform::uart_read_impl(hal_handle_t handle, uint8_t* buf, size_t len)
{
  (void)handle;
  (void)buf;
  (void)len;
  // Non-blocking read - simplified implementation returns 0
  return 0;
}

int PosixPlatform::uart_available_impl(hal_handle_t handle)
{
  (void)handle;
  return 0;  // Simplified implementation
}

/* ========================================================================= */
/* Timer Implementation                                                      */
/* ========================================================================= */

static uint64_t get_time_ns()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL + ts.tv_nsec;
}

static const uint64_t start_time = get_time_ns();

uint32_t PosixPlatform::millis_impl()
{
  return static_cast<uint32_t>((get_time_ns() - start_time) / 1000000);
}

uint64_t PosixPlatform::micros_impl()
{
  return (get_time_ns() - start_time) / 1000;
}

void PosixPlatform::delay_ms_impl(uint32_t ms)
{
  usleep(ms * 1000);
}

void PosixPlatform::delay_us_impl(uint32_t us)
{
  usleep(us);
}

/* ========================================================================= */
/* Console I/O Implementation                                                */
/* ========================================================================= */

int PosixPlatform::console_write_impl(const uint8_t* buf, size_t len)
{
  // Write to stdout using POSIX write()
  ssize_t written = write(STDOUT_FILENO, buf, len);
  return (written >= 0) ? static_cast<int>(written) : HAL_ERR_IO;
}

int PosixPlatform::console_read_impl(uint8_t* buf, size_t len)
{
  // Read from stdin using POSIX read() - blocking
  ssize_t bytes_read = read(STDIN_FILENO, buf, len);
  return (bytes_read >= 0) ? static_cast<int>(bytes_read) : HAL_ERR_IO;
}

/* ========================================================================= */
/* Interrupt Control Implementation                                          */
/* ========================================================================= */

// Static mutex for critical sections
static pthread_mutex_t critical_mutex = PTHREAD_MUTEX_INITIALIZER;

void PosixPlatform::critical_enter_impl()
{
  // Use pthread_mutex for thread-safe critical sections
  pthread_mutex_lock(&critical_mutex);
}

void PosixPlatform::critical_exit_impl()
{
  // Release mutex
  pthread_mutex_unlock(&critical_mutex);
}

}  // namespace hal
}  // namespace v4

/* ========================================================================= */
/* Platform Capabilities                                                     */
/* ========================================================================= */

extern "C" const hal_capabilities_t* hal_platform_capabilities(void)
{
  static const hal_capabilities_t caps = {
      32,  // gpio_count
      4,   // uart_count
      0,   // spi_count
      0,   // i2c_count
      0,   // has_adc
      0,   // has_dac
      0,   // has_pwm
      0,   // has_rtc
      0,   // has_dma
      0,   // reserved
  };
  return &caps;
}
