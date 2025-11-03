/**
 * @file platform_esp32.cpp
 * @brief ESP32 platform implementation for V4 HAL
 */

#include "platform_esp32.hpp"

#include "v4/hal_error.h"

// ESP-IDF includes
#ifdef HAL_PLATFORM_ESP32
#include <cstdio>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

namespace v4
{
namespace hal
{

#ifdef HAL_PLATFORM_ESP32

/* ========================================================================= */
/* GPIO Implementation                                                       */
/* ========================================================================= */

int Esp32Platform::gpio_mode_impl(int pin, hal_gpio_mode_t mode)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
    return HAL_ERR_PARAM;

  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = (1ULL << pin);
  io_conf.intr_type = GPIO_INTR_DISABLE;

  switch (mode)
  {
    case HAL_GPIO_INPUT:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;

    case HAL_GPIO_INPUT_PULLUP:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;

    case HAL_GPIO_INPUT_PULLDOWN:
      io_conf.mode = GPIO_MODE_INPUT;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
      break;

    case HAL_GPIO_OUTPUT:
      io_conf.mode = GPIO_MODE_OUTPUT;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;

    case HAL_GPIO_OUTPUT_OD:
      io_conf.mode = GPIO_MODE_OUTPUT_OD;
      io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
      io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
      break;

    default:
      return HAL_ERR_PARAM;
  }

  if (gpio_config(&io_conf) != ESP_OK)
    return HAL_ERR_IO;

  return HAL_OK;
}

int Esp32Platform::gpio_write_impl(int pin, hal_gpio_value_t value)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
    return HAL_ERR_PARAM;

  if (gpio_set_level(static_cast<gpio_num_t>(pin), value) != ESP_OK)
    return HAL_ERR_IO;

  return HAL_OK;
}

int Esp32Platform::gpio_read_impl(int pin, hal_gpio_value_t* value)
{
  if (pin < 0 || pin >= GPIO_NUM_MAX)
    return HAL_ERR_PARAM;
  if (!value)
    return HAL_ERR_PARAM;

  int level = gpio_get_level(static_cast<gpio_num_t>(pin));
  *value = static_cast<hal_gpio_value_t>(level);

  return HAL_OK;
}

/* ========================================================================= */
/* UART Implementation                                                       */
/* ========================================================================= */

// UART buffer sizes
#define UART_RX_BUF_SIZE 2048
#define UART_TX_BUF_SIZE 0  // No TX buffer (blocking)

// Track which UARTs have been initialized
static bool uart_init_flags[3] = {false, false, false};

hal_handle_t Esp32Platform::uart_open_impl(int port, const hal_uart_config_t* config)
{
  if (port < 0 || port >= max_uart_ports())
    return nullptr;
  if (!config)
    return nullptr;

  // Skip re-initialization
  if (uart_init_flags[port])
  {
    return reinterpret_cast<hal_handle_t>(static_cast<uintptr_t>(port + 1));
  }

  uart_config_t uart_config = {
      .baud_rate = config->baudrate,
      .data_bits = static_cast<uart_word_length_t>(config->data_bits - 5),
      .parity = static_cast<uart_parity_t>(config->parity),
      .stop_bits = static_cast<uart_stop_bits_t>(config->stop_bits - 1),
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0,
      .source_clk = UART_SCLK_DEFAULT,
  };

  uart_port_t uart_num = static_cast<uart_port_t>(port);

  if (uart_param_config(uart_num, &uart_config) != ESP_OK)
    return nullptr;

  // Use default pins for UART0 (USB-CDC on ESP32-C6)
  if (uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                   UART_PIN_NO_CHANGE) != ESP_OK)
    return nullptr;

  if (uart_driver_install(uart_num, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, 0, nullptr, 0) !=
      ESP_OK)
    return nullptr;

  uart_init_flags[port] = true;

  // Return port number as handle (cast to pointer)
  return reinterpret_cast<hal_handle_t>(static_cast<uintptr_t>(port + 1));
}

int Esp32Platform::uart_close_impl(hal_handle_t handle)
{
  if (!handle)
    return HAL_ERR_PARAM;

  int port = static_cast<int>(reinterpret_cast<uintptr_t>(handle)) - 1;
  if (port < 0 || port >= max_uart_ports())
    return HAL_ERR_PARAM;

  if (!uart_init_flags[port])
    return HAL_OK;  // Already closed

  uart_port_t uart_num = static_cast<uart_port_t>(port);

  if (uart_driver_delete(uart_num) != ESP_OK)
    return HAL_ERR_IO;

  uart_init_flags[port] = false;
  return HAL_OK;
}

int Esp32Platform::uart_write_impl(hal_handle_t handle, const uint8_t* buf, size_t len)
{
  if (!handle || !buf)
    return HAL_ERR_PARAM;

  int port = static_cast<int>(reinterpret_cast<uintptr_t>(handle)) - 1;
  if (port < 0 || port >= max_uart_ports())
    return HAL_ERR_PARAM;

  if (!uart_init_flags[port])
    return HAL_ERR_NODEV;

  uart_port_t uart_num = static_cast<uart_port_t>(port);

  int written = uart_write_bytes(uart_num, buf, len);
  return (written >= 0) ? written : HAL_ERR_IO;
}

int Esp32Platform::uart_read_impl(hal_handle_t handle, uint8_t* buf, size_t len)
{
  if (!handle || !buf)
    return HAL_ERR_PARAM;

  int port = static_cast<int>(reinterpret_cast<uintptr_t>(handle)) - 1;
  if (port < 0 || port >= max_uart_ports())
    return HAL_ERR_PARAM;

  if (!uart_init_flags[port])
    return HAL_ERR_NODEV;

  uart_port_t uart_num = static_cast<uart_port_t>(port);

  // Non-blocking read with 0 timeout
  int bytes_read = uart_read_bytes(uart_num, buf, len, 0);
  return (bytes_read >= 0) ? bytes_read : HAL_ERR_IO;
}

int Esp32Platform::uart_available_impl(hal_handle_t handle)
{
  if (!handle)
    return HAL_ERR_PARAM;

  int port = static_cast<int>(reinterpret_cast<uintptr_t>(handle)) - 1;
  if (port < 0 || port >= max_uart_ports())
    return HAL_ERR_PARAM;

  if (!uart_init_flags[port])
    return HAL_ERR_NODEV;

  uart_port_t uart_num = static_cast<uart_port_t>(port);

  size_t available = 0;
  if (uart_get_buffered_data_len(uart_num, &available) != ESP_OK)
    return HAL_ERR_IO;

  return static_cast<int>(available);
}

/* ========================================================================= */
/* Timer Implementation                                                      */
/* ========================================================================= */

uint32_t Esp32Platform::millis_impl()
{
  return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
}

uint64_t Esp32Platform::micros_impl()
{
  return static_cast<uint64_t>(esp_timer_get_time());
}

void Esp32Platform::delay_ms_impl(uint32_t ms)
{
  TickType_t ticks = pdMS_TO_TICKS(ms);
  if (ticks == 0 && ms > 0)
    ticks = 1;  // Minimum 1 tick delay
  vTaskDelay(ticks);
}

void Esp32Platform::delay_us_impl(uint32_t us)
{
  if (us == 0)
    return;

  // For delays > 1ms, use vTaskDelay for better CPU efficiency
  if (us >= 1000)
  {
    delay_ms_impl(us / 1000);
    return;
  }

  // Busy-wait for short delays
  int64_t start = esp_timer_get_time();
  while ((esp_timer_get_time() - start) < static_cast<int64_t>(us))
  {
    // Busy wait
  }
}

/* ========================================================================= */
/* Console I/O Implementation                                                */
/* ========================================================================= */

int Esp32Platform::console_write_impl(const uint8_t* buf, size_t len)
{
  if (!buf)
    return HAL_ERR_PARAM;

  // Use UART0 for console output
  int written = uart_write_bytes(UART_NUM_0, buf, len);
  return (written >= 0) ? written : HAL_ERR_IO;
}

int Esp32Platform::console_read_impl(uint8_t* buf, size_t len)
{
  if (!buf)
    return HAL_ERR_PARAM;

  // Blocking read from UART0
  int bytes_read = uart_read_bytes(UART_NUM_0, buf, len, portMAX_DELAY);
  return (bytes_read >= 0) ? bytes_read : HAL_ERR_IO;
}

/* ========================================================================= */
/* Interrupt Control Implementation                                          */
/* ========================================================================= */

// Static spinlock for critical sections
static portMUX_TYPE critical_spinlock = portMUX_INITIALIZER_UNLOCKED;

void Esp32Platform::critical_enter_impl()
{
  // Use FreeRTOS spinlock (disables interrupts on current core)
  // Supports nesting - each enter must be paired with exit
  portENTER_CRITICAL(&critical_spinlock);
}

void Esp32Platform::critical_exit_impl()
{
  // Re-enable interrupts
  portEXIT_CRITICAL(&critical_spinlock);
}

#else  // !HAL_PLATFORM_ESP32

/* Stub implementations for non-ESP32 builds */

int Esp32Platform::gpio_mode_impl(int, hal_gpio_mode_t)
{
  return HAL_ERR_NOTSUP;
}
int Esp32Platform::gpio_write_impl(int, hal_gpio_value_t)
{
  return HAL_ERR_NOTSUP;
}
int Esp32Platform::gpio_read_impl(int, hal_gpio_value_t*)
{
  return HAL_ERR_NOTSUP;
}
hal_handle_t Esp32Platform::uart_open_impl(int, const hal_uart_config_t*)
{
  return nullptr;
}
int Esp32Platform::uart_close_impl(hal_handle_t)
{
  return HAL_ERR_NOTSUP;
}
int Esp32Platform::uart_write_impl(hal_handle_t, const uint8_t*, size_t)
{
  return HAL_ERR_NOTSUP;
}
int Esp32Platform::uart_read_impl(hal_handle_t, uint8_t*, size_t)
{
  return HAL_ERR_NOTSUP;
}
int Esp32Platform::uart_available_impl(hal_handle_t)
{
  return HAL_ERR_NOTSUP;
}
uint32_t Esp32Platform::millis_impl()
{
  return 0;
}
uint64_t Esp32Platform::micros_impl()
{
  return 0;
}
void Esp32Platform::delay_ms_impl(uint32_t) {}
void Esp32Platform::delay_us_impl(uint32_t) {}
int Esp32Platform::console_write_impl(const uint8_t*, size_t)
{
  return HAL_ERR_NOTSUP;
}
int Esp32Platform::console_read_impl(uint8_t*, size_t)
{
  return HAL_ERR_NOTSUP;
}
void Esp32Platform::critical_enter_impl() {}
void Esp32Platform::critical_exit_impl() {}

#endif  // HAL_PLATFORM_ESP32

}  // namespace hal
}  // namespace v4
