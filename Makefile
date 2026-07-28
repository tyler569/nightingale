.PHONY: all release clean

all: debug

debug:
	cmake --preset debug
	cmake --build build -t iso

release:
	cmake --preset release
	cmake --build build-release -t iso

clean:
	[ -d build ] && cmake --build build -t clean
	[ -d build-release ] && cmake --build build-release -t clean
