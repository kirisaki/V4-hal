# V4-hal

Hardware Abstraction Layer (HAL) API definitions for the V4 Virtual Machine.

## Overview

**V4-hal** defines the standard HAL interface that enables V4 VM to run on various embedded hardware platforms. It provides API specifications for GPIO, UART, Timer, and System operations.

This repository contains:
- HAL API header definitions
- Comprehensive documentation
- Mock HAL implementation for testing

### Architecture

```
V4-ports (HAL implementations, platform-specific)
    ↓ depends on
V4-hal (HAL API definitions, this repository)
    ↑ depends on
V4 (VM core, uses HAL via SYS instruction)
V4-front (Forth compiler)
V4-repl (Interactive REPL)
```

### Implementation Architecture

V4-hal uses a layered architecture with C++17 internal implementation and extern "C" API:

```
┌─────────────────────────────────────────┐
│  Application (C/C++)                    │
├─────────────────────────────────────────┤
│  extern "C" API (hal.h, v4_hal.h)      │
├─────────────────────────────────────────┤
│  C API Bridge (src/bridge/)            │
│  - hal_gpio_bridge.cpp                 │
│  - hal_uart_bridge.cpp                 │
│  - hal_timer_bridge.cpp                │
├─────────────────────────────────────────┤
│  C++17 Internal Layer (src/internal/)  │
│  - CRTP templates                      │
│  - Zero-cost abstractions              │
│  - Type-safe wrappers                  │
├─────────────────────────────────────────┤
│  Platform Layer (ports/)               │
│  - posix/    (Linux/macOS)            │
│  - esp32/    (ESP32-C6)               │
│  - ch32v203/ (CH32V203)               │
└─────────────────────────────────────────┘
```

**Design Principles:**
- **External Interface**: Pure C API (extern "C") for maximum compatibility
- **Internal Implementation**: C++17 with templates, CRTP, and RAII
- **Zero Cost**: No exceptions, no RTTI, inline templates
- **Type Safety**: Compile-time checks with C++ type system
- **Resource Efficiency**: Stack-only, no dynamic allocation

## Directory Structure

```
V4-hal/
├── include/v4/           # Public API headers
│   ├── v4_hal.h         # Legacy C API
│   ├── hal.h            # Unified C++17 API (planned)
│   ├── hal_types.h      # Type definitions (planned)
│   ├── hal_error.h      # Error codes (planned)
│   └── hal_capabilities.h  # Capability system (planned)
├── src/
│   ├── internal/        # C++17 templates (planned)
│   │   ├── gpio_impl.hpp
│   │   ├── uart_impl.hpp
│   │   └── timer_impl.hpp
│   ├── bridge/          # extern "C" bridge (planned)
│   │   ├── hal_gpio_bridge.cpp
│   │   ├── hal_uart_bridge.cpp
│   │   └── hal_timer_bridge.cpp
│   └── common/          # Core implementation (planned)
│       ├── hal_core.cpp
│       └── hal_capabilities.cpp
├── ports/               # Platform implementations (planned)
│   ├── posix/          # Linux/macOS
│   ├── esp32/          # ESP32-C6
│   └── ch32v203/       # CH32V203
├── tests/
│   ├── mock_hal.cpp    # Mock implementation
│   ├── test_mock_hal.cpp  # Unit tests
│   └── unit/           # Additional tests (planned)
├── examples/
│   └── blink/          # LED blink example (planned)
└── docs/               # Documentation
    ├── hal-api.md
    └── sys-opcodes.md
```

## Features

- Minimal API surface for embedded systems
- No dynamic allocation
- Consistent error codes (v4_err)
- Platform-agnostic interface
- C linkage for maximum compatibility

## HAL API Categories

### GPIO Control
- `v4_hal_gpio_init()` - Initialize GPIO pin with mode
- `v4_hal_gpio_write()` - Write digital output
- `v4_hal_gpio_read()` - Read digital input

### UART Communication
- `v4_hal_uart_init()` - Initialize UART with baud rate
- `v4_hal_uart_putc()` - Send single character
- `v4_hal_uart_getc()` - Receive single character (non-blocking)
- `v4_hal_uart_write()` - Write buffer
- `v4_hal_uart_read()` - Read buffer (non-blocking)

### Timer Operations
- `v4_hal_millis()` - Get milliseconds since startup
- `v4_hal_micros()` - Get microseconds since startup (64-bit)
- `v4_hal_delay_ms()` - Blocking delay in milliseconds
- `v4_hal_delay_us()` - Blocking delay in microseconds

### System Control
- `v4_hal_system_reset()` - Reset microcontroller
- `v4_hal_system_info()` - Get platform information string

## Usage

### For Platform Implementers (V4-ports)

Include the HAL header and implement all required functions:

```c
#include "v4/v4_hal.h"

v4_err v4_hal_gpio_init(int pin, v4_hal_gpio_mode mode) {
    // Platform-specific GPIO initialization
    // ...
    return 0;  // Success
}

// Implement other HAL functions...
```

See [V4-ports](https://github.com/kirisaki/V4-ports) for reference implementations.

### For VM Users

HAL functions are accessed via the `SYS` instruction in V4 bytecode:

```forth
\ Blink LED on pin 13
: SETUP 13 1 SYS 0x00 DROP ;     \ Init pin as output
: BLINK
  13 1 SYS 0x01 DROP             \ LED on
  500 SYS 0x22                   \ Delay 500ms
  13 0 SYS 0x01 DROP             \ LED off
  500 SYS 0x22                   \ Delay 500ms
;
: MAIN SETUP BEGIN BLINK AGAIN ;
```

## Documentation

- [HAL API Reference](docs/hal-api.md) - Complete API documentation
- [SYS Instruction Opcodes](docs/sys-opcodes.md) - SYS ID assignments and usage examples

## Mock HAL for Testing

V4-hal includes a mock HAL implementation for unit testing V4 core without hardware:

```c
#include "mock_hal.h"

// Mock records all HAL calls for verification
v4_hal_gpio_write(13, 1);
assert(mock_gpio_state[13] == 1);
```

## Platform Support

| Platform | Repository | Status |
|----------|------------|--------|
| ESP32-C6 | [V4-ports](https://github.com/kirisaki/V4-ports) | Stable |
| CH32V | V4-ports | Planned |
| RP2350 | V4-ports | Planned |
| Mock HAL | V4-hal (this repo) | For testing |

## Building

V4-hal is a header-only library. Simply include the headers in your project:

```cmake
# CMakeLists.txt
add_subdirectory(V4-hal)
target_link_libraries(your_target INTERFACE v4-hal)
```

Or manually specify the include path:

```bash
gcc -I/path/to/V4-hal/include your_code.c
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Format code with `make format`
4. Submit a pull request

### Coding Standards

- Follow existing code style
- Use C language for HAL definitions
- Add documentation for new APIs
- Update CHANGELOG.md

## Related Projects

- [V4](https://github.com/kirisaki/v4) - VM core implementation
- [V4-front](https://github.com/kirisaki/v4-front) - Forth compiler
- [V4-repl](https://github.com/kirisaki/v4-repl) - Interactive REPL
- [V4-ports](https://github.com/kirisaki/v4-ports) - Platform-specific HAL implementations

## License

Licensed under either of:

- MIT License ([LICENSE-MIT](LICENSE-MIT))
- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE))

at your option.

## Version

Current version: 0.1.0

---

**Status**: Active Development | **Last Updated**: 2025
