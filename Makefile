
all:
	nasm -f elf32 kernel.asm -o kasm.o
	nasm -f elf32 move_cursor.asm -o move_cursor.o
	gcc -m32 -c kernel.c -o kc.o
	gcc -m32 -c utils.c -o utils.o
	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o utils.o move_cursor.o

run: all
	qemu-system-i386 -kernel kernel -curses
