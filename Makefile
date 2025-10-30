.PHONY: help build test clean format format-check

help:
	@echo "V4-hal Makefile"
	@echo ""
	@echo "Usage:"
	@echo "  make build        Build mock HAL and tests"
	@echo "  make test         Run tests"
	@echo "  make clean        Clean build artifacts"
	@echo "  make format       Format code with clang-format"
	@echo "  make format-check Check code formatting"
	@echo "  make help         Show this help message"

build:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug -DV4_HAL_BUILD_MOCK=ON -DV4_HAL_BUILD_TESTS=ON
	cmake --build build -j

test: build
	cd build && ctest --output-on-failure

clean:
	rm -rf build

format:
	find include tests -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cpp' | xargs clang-format -i

format-check:
	find include tests -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cpp' | xargs clang-format --dry-run --Werror
