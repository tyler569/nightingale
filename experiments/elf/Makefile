CFLAGS := -Wall -Wextra -g -Og -std=c23 -fsanitize=address
LDFLAGS := -g -fsanitize=address

all: module.o main

module.o:
	clang -c -o module.o module.c -O0 -target x86_64-unknown-none -nostdlib

clean:
	rm -rf module.o main
