
global move_cursor

section .text

move_cursor:
    push eax
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 0x02
    mov dl, 0x02
    int 0x10
    pop eax
    ret
