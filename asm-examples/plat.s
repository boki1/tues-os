	global f
	global swap
	global g
	global memcpy_dummy
	global bulk_sum
	global max
	global add_many
	global sub_many
	global _ZplRK2ssS1_
	global _Zpl5vec2dS_
	global zzz, const_zzz, zeroed_zzz

	segment .bss
zeroed_zzz:	resd 1

	segment .rdata
const_zzz:	dq 0x1210

	segment .data
zzz:	dd 	0x1234

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

sub_many:
	ret

_ZplRK2ssS1_:
	xor rax, rax
	ret

	struc vec2d
vec_x: resq 1
vec_y: resq 1
	endstruc

_Zpl5vec2dS_:
	movsd xmm0, [rdi+vec_x]
	addsd xmm0, [rsi+vec_x]
	movsd [rdi+vec_x], xmm0
	movsd xmm0, [rdi+vec_y]
	addsd xmm0, [rsi+vec_y]
	movsd [rdi+vec_y], xmm0
	ret
