/**
 * @file blink.cpp
 * @brief Simple LED blink example using V4-hal
 *
 * Demonstrates basic GPIO and Timer usage with the V4-hal API.
 */

#include <cstdio>

#include "v4/hal.h"

// Pin configuration
constexpr int LED_PIN = 13;
constexpr uint32_t BLINK_INTERVAL_MS = 1000;

int main(void)
{
  printf("V4-hal Blink Example\n");
  printf("====================\n\n");

  // Initialize HAL
  int ret = hal_init();
  if (ret != HAL_OK)
  {
    printf("Error: Failed to initialize HAL (error %d)\n", ret);
    return 1;
  }

  // Print platform capabilities
  const hal_capabilities_t* caps = hal_get_capabilities();
  printf("Platform capabilities:\n");
  printf("  GPIO pins: %u\n", caps->gpio_count);
  printf("  UART ports: %u\n", caps->uart_count);
  printf("\n");

  // Configure LED pin as output
  ret = hal_gpio_mode(LED_PIN, HAL_GPIO_OUTPUT);
  if (ret != HAL_OK)
  {
    printf("Error: Failed to configure GPIO pin %d (error %d)\n", LED_PIN, ret);
    return 1;
  }

  printf("Blinking LED on pin %d...\n", LED_PIN);
  printf("Press Ctrl+C to exit\n\n");

  // Blink loop
  hal_gpio_value_t state = HAL_GPIO_LOW;
  for (int i = 0; i < 10; i++)  // Blink 10 times for demo
  {
    // Toggle LED
    state = (state == HAL_GPIO_LOW) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
    ret = hal_gpio_write(LED_PIN, state);
    if (ret != HAL_OK)
    {
      printf("Error: Failed to write GPIO pin %d (error %d)\n", LED_PIN, ret);
      break;
    }

    printf("[%u ms] LED %s\n", hal_millis(), (state == HAL_GPIO_HIGH) ? "ON " : "OFF");

    // Delay
    hal_delay_ms(BLINK_INTERVAL_MS);
  }

  printf("\nBlink complete!\n");

  // Cleanup
  hal_deinit();

  return 0;
}
