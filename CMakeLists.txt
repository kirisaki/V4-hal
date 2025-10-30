cmake_minimum_required(VERSION 3.15)
project(v4-hal VERSION 0.1.0 LANGUAGES C CXX)

# C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Header-only interface library
add_library(v4-hal INTERFACE)
target_include_directories(v4-hal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Optional: Build mock HAL implementation for testing
option(V4_HAL_BUILD_MOCK "Build mock HAL implementation" OFF)

if(V4_HAL_BUILD_MOCK)
  add_library(v4-hal-mock STATIC
    tests/mock_hal.cpp
  )
  target_include_directories(v4-hal-mock PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
  )
  target_compile_options(v4-hal-mock PRIVATE
    -Wall -Wextra -Wpedantic
    -fno-exceptions -fno-rtti
  )
endif()

# Optional: Build tests
option(V4_HAL_BUILD_TESTS "Build HAL tests" OFF)

if(V4_HAL_BUILD_TESTS)
  enable_testing()

  # Fetch doctest
  include(FetchContent)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
  )
  FetchContent_MakeAvailable(doctest)

  # Test executable
  add_executable(test_hal
    tests/test_mock_hal.cpp
  )
  target_link_libraries(test_hal PRIVATE
    v4-hal-mock
    doctest::doctest
  )
  target_compile_options(test_hal PRIVATE
    -Wall -Wextra -Wpedantic
    -fno-exceptions -fno-rtti
  )

  add_test(NAME test_hal COMMAND test_hal)
endif()

# Installation
install(DIRECTORY include/ DESTINATION include)
install(FILES
  LICENSE-MIT
  LICENSE-APACHE
  README.md
  DESTINATION share/doc/v4-hal
)
