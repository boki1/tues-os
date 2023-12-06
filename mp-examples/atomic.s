	global _ZN6atomicIlEppEv ; Int operator++();
	global _ZN6atomicIlEppEi ; Int operator++(int);
	global _ZN6atomicIlEmmEv ; Int operator--();
	global _ZN6atomicIlEmmEi ; Int operator--(int);
	global _ZN6atomicIlE9fetch_addEl ; Int fetch_add(Int);
	global _ZN6atomicIlE9fetch_subEl ; Int fetch_sub(Int);
	global _ZN6atomicIlE9fetch_andEl ; Int fetch_and(Int);
	global _ZN6atomicIlE8fetch_orEl ; Int fetch_or(Int);
	global _ZN6atomicIlE9fetch_xorEl ; Int fetch_xor(Int);
	global _ZNK6atomicIlE4loadEv ; Int load() const;
	global _ZN6atomicIlE5storeEl ; void store(Int);
	global _ZN6atomicIlE21compare_exchange_weakERll ; Int compare_exchange_weak(Int &expected, Int desired);

	segment .text

_ZN6atomicIlEppEv: ; Int operator++();
	mov r8, 1
	lock xadd [rdi], r8
	mov rax, r8
	inc rax
	ret

_ZN6atomicIlEppEi: ; Int operator++(int);
	mov r8, 1
	lock xadd [rdi], r8
	mov rax, r8
	ret

_ZN6atomicIlEmmEv: ; Int operator--();
	mov r8, -1
	lock xadd [rdi], r8
	mov rax, r8
	dec rax
	ret

_ZN6atomicIlEmmEi: ; Int operator--(int);
	mov r8, -1
	lock xadd [rdi], r8
	mov rax, r8
	ret

_ZN6atomicIlE9fetch_addEl: ; Int fetch_add(Int);
	lock xadd [rdi], rsi
	mov rax, rsi
	ret

_ZN6atomicIlE9fetch_subEl: ; Int fetch_sub(Int);
	not rsi
	inc rsi
	lock xadd [rdi], rsi
	mov rax, rsi
	ret

_ZN6atomicIlE9fetch_andEl: ; Int fetch_and(Int);
	mov rax, [rdi]
cas_loop_fetch_and:
	mov r8, rax
	and r8, rsi
	lock cmpxchg [rdi], r8
	sete cl
	test cl, cl
	je cas_loop_fetch_and
	ret

_ZN6atomicIlE8fetch_orEl: ; Int fetch_or(Int);
	mov rax, [rdi]
cas_loop_fetch_or:
	mov r8, rax
	or r8, rsi
	lock cmpxchg [rdi], r8
	sete cl
	test cl, cl
	je cas_loop_fetch_or
	ret

_ZN6atomicIlE9fetch_xorEl: ; Int fetch_xor(Int);
	mov rax, [rdi]
cas_loop_fetch_xor:
	mov r8, rax
	xor r8, rsi
	lock cmpxchg [rdi], r8
	sete cl
	test cl, cl
	je cas_loop_fetch_xor
	ret

_ZNK6atomicIlE4loadEv: ; Int load() const;
	mov rax, [rdi]
	ret

_ZN6atomicIlE5storeEl: ; void store(Int);
	mov [rdi], rsi
	ret

_ZN6atomicIlE21compare_exchange_weakERll: ; Int compare_exchange_weak(this rdi, Int &expected_rsi, Int desired_rdx);
	mov rax, [rsi]
	lock cmpxchg [rdi], rdx
	sete al
	movzx rax, al
	ret
