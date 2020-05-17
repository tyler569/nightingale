
.PHONY: mod.o

all: demo mod.o link

link: link.c elf.c
	$(CC) $(CFLAGS) $(INCLUDE) link.c elf.c -o link

demo: demo.c elf.c
	$(CC) $(CFLAGS) $(INCLUDE) demo.c elf.c -static -o demo

mod.o:
	$(CC) -c mod.c -o mod.o

clean:
	rm -f demo mod.o link elf.o
