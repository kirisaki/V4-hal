add_executable(blink blink.cpp)
target_link_libraries(blink PRIVATE v4-hal-lib)
target_compile_options(blink PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti)
