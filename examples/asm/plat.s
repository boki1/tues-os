	global f
	global swap
	global g
	global memcpy_dummy
	global bulk_sum

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
	sub rdx, 1
	jnz memcpy_dummy
	ret

bulk_sum:
	movsx rax, dword [rdi]
	add rax, rsi
	add rax, rdx
	add rax, rcx
	add rax, r8
	add rax, r9
	add rax, [rsp+0x8]
	mov rsi, [rsp+0x10]
	add rax, [rsi]
	ret
