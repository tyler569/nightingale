
.global elf_lazy_resolve_stub
elf_lazy_resolve_stub:
    // push n
    // push GOT[1]
    pop %r11 // GOT[1]
    pop %r10 // n

    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %r8
    push %r9

    mov %r11, %rdi
    mov %r10, %rsi

    call elf_lazy_resolve

    mov %rax, %r11

    pop %r9
    pop %r8
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi

    jmp *%r11

