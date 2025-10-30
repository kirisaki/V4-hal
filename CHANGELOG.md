# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
