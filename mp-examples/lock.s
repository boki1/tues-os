
	global _ZN4lock7acquireEv
	global _ZN4lock7releaseEv

	section .text

_ZN4lock7acquireEv:
	lock bts word [rdi], 0
	mov rax, 1
	sbb rax, rax
	neg rax
	ret

_ZN4lock7releaseEv:
	lock btr word [rdi], 0
	ret
