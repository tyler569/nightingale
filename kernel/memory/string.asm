
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

global memset
memset:
	mov al, sil
	mov rcx, rdx
	rep stosb

	ret

global wmemset
wmemset:
	mov ax, si
	mov rcx, rdx
	rep stosw
	
	ret

global strlen
strlen:
	mov rdx, rdi
	xor ecx, ecx
	not rcx ; rcx = 0xFFFFFFFFFFFFFFFF
	xor eax, eax

	repne scasb
	
	mov rax, rdi
	sub rax, rdx

	ret
	
