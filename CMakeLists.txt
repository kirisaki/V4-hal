cmake_minimum_required(VERSION 3.15)
project(
  v4-hal
  VERSION 0.1.0
  LANGUAGES C CXX)

# C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only interface library (for header-only usage)
add_library(v4-hal INTERFACE)
target_include_directories(
  v4-hal INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   $<INSTALL_INTERFACE:include>)

# Platform selection
set(HAL_PLATFORM
    "posix"
    CACHE STRING "Target platform (posix, esp32, ch32v203)")
set_property(CACHE HAL_PLATFORM PROPERTY STRINGS posix esp32 ch32v203)

# Static library with C++17 CRTP implementation
add_library(
  v4-hal-lib STATIC
  src/common/hal_capabilities.cpp src/common/hal_core.cpp src/bridge/hal_gpio_bridge.cpp
  src/bridge/hal_uart_bridge.cpp src/bridge/hal_timer_bridge.cpp)

# Platform-specific sources
if(HAL_PLATFORM STREQUAL "posix")
  target_sources(v4-hal-lib PRIVATE ports/posix/platform_posix.cpp)
  target_compile_definitions(v4-hal-lib PRIVATE HAL_PLATFORM_POSIX)
  target_include_directories(v4-hal-lib PRIVATE ports/posix)
elseif(HAL_PLATFORM STREQUAL "esp32")
  target_sources(v4-hal-lib PRIVATE ports/esp32/platform_esp32.cpp)
  target_compile_definitions(v4-hal-lib PRIVATE HAL_PLATFORM_ESP32)
  target_include_directories(v4-hal-lib PRIVATE ports/esp32)
elseif(HAL_PLATFORM STREQUAL "ch32v203")
  target_sources(v4-hal-lib PRIVATE ports/ch32v203/platform_ch32v203.cpp)
  target_compile_definitions(v4-hal-lib PRIVATE HAL_PLATFORM_CH32V203)
  target_include_directories(v4-hal-lib PRIVATE ports/ch32v203)
else()
  message(FATAL_ERROR "Unsupported platform: ${HAL_PLATFORM}")
endif()

# Include directories for v4-hal-lib
target_include_directories(
  v4-hal-lib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:include>)
target_include_directories(v4-hal-lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Compiler options for v4-hal-lib
target_compile_options(
  v4-hal-lib PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti -Os # Optimize for
                                                                            # size
)

# Optional: Build mock HAL implementation for testing
option(V4_HAL_BUILD_MOCK "Build mock HAL implementation" OFF)

if(V4_HAL_BUILD_MOCK)
  add_library(v4-hal-mock STATIC tests/mock_hal.cpp)
  target_include_directories(v4-hal-mock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                ${CMAKE_CURRENT_SOURCE_DIR}/tests)
  target_compile_options(v4-hal-mock PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions
                                             -fno-rtti)
endif()

# Optional: Build tests
option(V4_HAL_BUILD_TESTS "Build HAL tests" OFF)

if(V4_HAL_BUILD_TESTS)
  enable_testing()

  # Fetch doctest
  include(FetchContent)
  fetchcontent_declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG master)
  fetchcontent_makeavailable(doctest)

  # Test executable
  add_executable(test_hal tests/test_mock_hal.cpp)
  target_link_libraries(test_hal PRIVATE v4-hal-mock doctest::doctest)
  target_compile_options(test_hal PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions
                                          -fno-rtti)

  add_test(NAME test_hal COMMAND test_hal)
endif()

# Optional: Build examples
option(V4_HAL_BUILD_EXAMPLES "Build HAL examples" OFF)

if(V4_HAL_BUILD_EXAMPLES)
  add_subdirectory(examples/blink)
endif()

# Installation
install(DIRECTORY include/ DESTINATION include)
install(FILES LICENSE-MIT LICENSE-APACHE README.md DESTINATION share/doc/v4-hal)
