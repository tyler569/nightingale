
bits 64
global memcpy
memcpy:
    push rdx
    cmp rdx, 0
    jz .done
	mov rcx, rdx

	rep movsb

.done:
    pop rax
    ret

_reverse_memcpy:
    push rdx
	cmp rdx, 0
	jz .done
	mov rcx, rdx

	add rdi, rcx
	add rsi, rcx

	std
	rep movsb
	cld

.done:
	pop rax
	ret
    

global memmove
memmove:
    cmp rsi, rdi
	jg .fwd
	jl .rev
	je .nop

.fwd:
	call memcpy
	jmp .done

.rev:
	call _reverse_memcpy
	jmp .done

.nop:
.done:
	ret

