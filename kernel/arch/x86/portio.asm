
bits 64
global inb
inb:
	push rdx
	mov rdx, rdi
	in al, dx
	pop rdx
	ret

global outb
outb:
	push rdx
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	pop rdx
	ret

global inw
inw:
	push rdx
	mov rdx, rdi
	in ax, dx
	pop rdx
	ret

global outw
outw:
	push rdx
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	pop rdx
	ret

global ind
ind:
	push rdx
	mov rdx, rdi
	in eax, dx
	pop rdx
	ret

global outd
outd:
	push rdx
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	pop rdx
	ret


