Disassembly of section .text:

<never>:
	xor    %eax,%eax
	xor    %ecx,%ecx
	cmp    %eax,%ecx
	je     <never+0xe>
	mov    $0x1,%eax
	ret
	mov    $0x2,%eax
	ret
	nopl   0x0(%rax)

<not_zero>:
	mov    $0x1,%eax
	ret
