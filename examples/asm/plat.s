	global f
	global swap
	global g
	global memcpy_dummy
	global bulk_sum
	global max
	global add_many

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

max:
	cmp rdi, rsi
	cmovb rdi, rsi
	mov rax, rdi
	ret

add_many: ; res is rdi, l is rsi, r is rdx, cnt is rcx
	clc
loop:
	mov r8, [rsi + (rcx - 1) * 8]
	adc r8, [rdx + (rcx - 1) * 8]
	mov [rdi + (rcx - 1) * 8], r8
	dec rcx
	jnz loop
	ret
