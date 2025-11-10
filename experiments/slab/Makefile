CC=clang
CFLAGS=-std=c23 -Wall -Wextra -Werror -Wno-unused-parameter -fsanitize=undefined -g
LDFLAGS=-fsanitize=undefined -g

.PHONY: all clean
all: slab slab_test
clean:
	rm -f slab slab_test *.o

slab: slab.o slab_main.o

slab_test: slab.o slab_test.o
