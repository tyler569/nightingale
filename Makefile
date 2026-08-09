.PHONY: all release clean

all: clang-debug

all-debug: clang-debug gcc-debug
everything: clang-debug clang-release gcc-debug gcc-release

clang-debug:
	cmake --preset clang-debug
	cmake --build build-clang-debug -t iso

clang-release:
	cmake --preset clang-release
	cmake --build build-clang-release -t iso

gcc-debug:
	cmake --preset gcc-debug
	cmake --build build-gcc-debug -t iso

gcc-release:
	cmake --preset gcc-release
	cmake --build build-gcc-release -t iso

clean:
	[ -d build-clang-debug ] && cmake --build build-clang-debug -t clean
	[ -d build-clang-release ] && cmake --build build-clang-release -t clean
	[ -d build-gcc-debug ] && cmake --build build-gcc-debug -t clean
	[ -d build-gcc-release ] && cmake --build build-gcc-release -t clean
