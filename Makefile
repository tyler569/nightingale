all:
	cmake -Bbuild -H. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=toolchain/CMake/CMakeToolchain.txt -G Ninja
	cmake --build build --config Debug

clean:
	rm -rf build build-* cmake-build-debug
