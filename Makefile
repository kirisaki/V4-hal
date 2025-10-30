.PHONY: all build release test clean format format-check help

# Platform selection (default: POSIX)
PLATFORM ?= posix

# Default target
all: build

# Debug build
build:
	@echo "🔨 Building V4-hal (Debug, Platform: $(PLATFORM))..."
	@cmake -B build -DCMAKE_BUILD_TYPE=Debug \
		-DV4_HAL_BUILD_MOCK=ON \
		-DV4_HAL_BUILD_TESTS=ON \
		-DHAL_PLATFORM=$(PLATFORM)
	@cmake --build build -j
	@echo "✅ Build complete!"

# Release build
release:
	@echo "🚀 Building V4-hal (Release, Platform: $(PLATFORM))..."
	@cmake -B build-release -DCMAKE_BUILD_TYPE=Release \
		-DV4_HAL_BUILD_MOCK=ON \
		-DV4_HAL_BUILD_TESTS=OFF \
		-DHAL_PLATFORM=$(PLATFORM)
	@cmake --build build-release -j
	@echo "✅ Release build complete!"

# Run tests
test: build
	@echo "🧪 Running tests..."
	@cd build && ctest --output-on-failure
	@echo "✅ All tests passed!"

# Clean
clean:
	@echo "🧹 Cleaning..."
	@rm -rf build build-release

# Format code
format:
	@echo "✨ Formatting C/C++ code..."
	@find include src ports tests examples -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cpp' \) \
		2>/dev/null | xargs clang-format -i
	@echo "✨ Formatting CMake files..."
	@find . -name 'CMakeLists.txt' -o -name '*.cmake' | xargs cmake-format -i 2>/dev/null || \
		echo "⚠️  cmake-format not found, skipping CMake formatting"
	@echo "✅ Formatting complete!"

# Check formatting
format-check:
	@echo "🔍 Checking C/C++ formatting..."
	@find include src ports tests examples -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cpp' \) \
		2>/dev/null | xargs clang-format --dry-run --Werror || \
		(echo "❌ C/C++ formatting check failed. Run 'make format' to fix." && exit 1)
	@echo "🔍 Checking CMake formatting..."
	@find . -name 'CMakeLists.txt' -o -name '*.cmake' | xargs cmake-format --check 2>/dev/null || \
		echo "⚠️  cmake-format not found, skipping CMake format check"
	@echo "✅ All formatting checks passed!"

# Help
help:
	@echo "V4-hal Makefile targets:"
	@echo ""
	@echo "  make / make all      - Build V4-hal library (Debug)"
	@echo "  make build           - Build debug version with tests"
	@echo "  make release         - Build optimized release version"
	@echo "  make test            - Run tests (requires build)"
	@echo "  make clean           - Remove build directories"
	@echo "  make format          - Format code with clang-format"
	@echo "  make format-check    - Check formatting without modifying files"
	@echo "  make help            - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  PLATFORM             - Target platform (default: posix)"
	@echo ""
	@echo "Examples:"
	@echo "  make                 # Build debug with POSIX platform"
	@echo "  make release         # Build optimized release"
	@echo "  make test            # Run all tests"
	@echo "  make PLATFORM=esp32  # Build for ESP32 platform (future)"
