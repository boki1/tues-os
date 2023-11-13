	global f
	global swap
	global g
	global memcpy_dummy

	segment .text
f:
	push rbp
	mov rbp, rsp
	sub rsp, 0x100
	mov [rsp+8], rcx
	mov rax, rcx
	pop rbp
	ret

swap:
	mov rax, [rdi]
	xchg rax, [rsi]
	mov [rdi], rax
	ret

g:
	mov rax, [rdi + rsi * 8]
	ret

memcpy_dummy:
	mov al, [rsi]
	mov [rdi], al
	add rdi, 1
	add rsi, 1
	sub rdx , 1
	jnz memcpy_dummy
	ret
