
#define SYS_EXIT 1

int _start() {
    int code = SYS_EXIT;
    asm volatile ("int $0x80" :: "A"(code));
}

