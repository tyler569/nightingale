
bits 64
global memcpy
memcpy:
    push rdx
    cmp rdx, 0
    jz .done

.byte:
    mov al, byte [rsi]
    stosb
    dec rdx
    jz .done
    jmp .byte

.done:
    pop rax
    ret

_reverse_memcpy:
    push rdx
    

global memmove
memmove:
    
