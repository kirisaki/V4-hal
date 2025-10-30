# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Directory structure for C++17 implementation
  - `src/internal/` for C++17 templates
  - `src/bridge/` for extern "C" bridge layer
  - `src/common/` for core implementation
  - `ports/posix/` for POSIX platform
  - `examples/blink/` for examples
  - `tests/unit/` for unit tests
- Implementation architecture documentation in README
- Design principles (CRTP, zero-cost abstractions, RAII)
- Type definitions (`include/v4/hal_types.h`)
  - GPIO types: mode, value, IRQ edge, IRQ handler
  - UART types: config structure
  - Opaque handle type
- Error code definitions (`include/v4/hal_error.h`)
  - Standard error codes (OK, PARAM, BUSY, TIMEOUT, etc.)
- Capability system (`include/v4/hal_capabilities.h`, `src/common/hal_capabilities.cpp`)
  - Runtime platform feature discovery
  - Resource count queries (GPIO, UART, SPI, I2C)
  - Feature flags (ADC, DAC, PWM, RTC, DMA)
  - Weak symbol pattern for platform customization
- Unified HAL API header (`include/v4/hal.h`)
  - Complete API: GPIO, UART, Timer, System control
  - GPIO interrupts (attach, detach, enable, disable)
  - UART handle-based API
  - Initialization and reset functions
  - Comprehensive documentation
- GPIO internal layer with CRTP (`src/internal/gpio_impl.hpp`)
  - GpioBase<Platform>: CRTP base class for GPIO operations
  - Zero-cost abstraction with compile-time polymorphism
  - Parameter validation (pin bounds checking)
  - Toggle operation (atomic read-modify-write)
  - GpioPin<Platform>: RAII wrapper for safe pin management
  - Move semantics support, copy prohibited
- UART internal layer with CRTP (`src/internal/uart_impl.hpp`)
  - UartBase<Platform>: CRTP base class for UART operations
  - Handle-based API for multiple port management
  - UartHandle: Opaque handle structure with platform data
  - open/close/write/read/available operations
  - Uart<Platform>: RAII wrapper with automatic resource cleanup
  - Move semantics support, copy prohibited
- Timer internal layer with CRTP (`src/internal/timer_impl.hpp`)
  - TimerBase<Platform>: CRTP base class for timer operations
  - millis(): Get milliseconds since startup (32-bit, wraps after 49 days)
  - micros(): Get microseconds since startup (64-bit)
  - delay_ms()/delay_us(): Blocking delays
  - elapsed_ms()/elapsed_us(): Helper functions for time measurement
- POSIX platform implementation (`ports/posix/`)
  - PosixPlatform: CRTP implementation for Linux/macOS/BSD
  - GPIO simulation using bitmaps (32 pins)
  - UART using stdout/stdin (4 ports)
  - Timer using clock_gettime(CLOCK_MONOTONIC)
  - Platform capabilities: 32 GPIO, 4 UART ports
  - Compile-time constants: max_gpio_pins(), max_uart_ports()
- extern "C" bridge layer for GPIO (`src/bridge/hal_gpio_bridge.cpp`)
  - Bridges C API (hal.h) to C++17 internal implementation
  - Platform selection via compile-time macros
  - hal_gpio_mode/write/read/toggle implementations
  - GPIO interrupt stubs (HAL_ERR_NOTSUP)
- extern "C" bridge layer for UART and Timer (`src/bridge/`)
  - hal_uart_bridge.cpp: UART operations (open/close/write/read/available)
  - hal_timer_bridge.cpp: Timer operations (millis/micros/delay_ms/delay_us)
  - Same platform selection mechanism as GPIO bridge
- Core initialization (`src/common/hal_core.cpp`)
  - hal_init(): Initialize HAL system with platform hooks
  - hal_reset(): Reset HAL system to initial state
  - hal_deinit(): Cleanup and release HAL resources
  - Weak symbol hooks: hal_platform_init/reset/deinit
  - Platform implementations can override for custom initialization
- Makefile build system (CMake wrapper)
  - CMake wrapper with convenient targets
  - Targets: build (debug), release, test, clean, format, format-check, help
  - Platform selection: PLATFORM=posix (default)
  - Options: V4_HAL_BUILD_TESTS, V4_HAL_BUILD_EXAMPLES, V4_HAL_BUILD_MOCK
- CMakeLists.txt updates
  - v4-hal-lib static library target with all C++17 CRTP sources
  - Platform selection via HAL_PLATFORM cache variable
  - Compiler flags: -std=c++17 -fno-exceptions -fno-rtti -Os
  - Fixed C++17 compatibility (aggregate initialization without designators)
  - libv4-hal-lib.a: 95KB static library (contains all peripheral implementations)
- Blink example (`examples/blink/`)
  - Demonstrates GPIO and Timer API usage
  - Simple LED blink pattern with 1s interval
  - Shows hal_init/deinit lifecycle
  - Platform capability queries
  - Binary footprint: 49KB (with debug symbols), actual runtime: ~5.7KB
  - Demonstrates linker dead code elimination (95KB library → 5.7KB binary)

## [0.1.0] - 2025-10-31

### Added
- Initial release of V4-hal
- HAL API header definition (`v4/v4_hal.h`)
- GPIO API: `v4_hal_gpio_init`, `v4_hal_gpio_write`, `v4_hal_gpio_read`
- UART API: `v4_hal_uart_init`, `v4_hal_uart_putc`, `v4_hal_uart_getc`, `v4_hal_uart_write`, `v4_hal_uart_read`
- Timer API: `v4_hal_millis`, `v4_hal_micros`, `v4_hal_delay_ms`, `v4_hal_delay_us`
- System API: `v4_hal_system_reset`, `v4_hal_system_info`
- Mock HAL implementation for testing
- Comprehensive documentation (HAL API reference, SYS opcodes)
- CMake build system with header-only interface library
- Unit tests using doctest

[Unreleased]: https://github.com/kirisaki/V4-hal/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/kirisaki/V4-hal/releases/tag/v0.1.0
