.PHONY: all

all:
	bash script/build_iso.bash

clean:
	ninja -C build clean
