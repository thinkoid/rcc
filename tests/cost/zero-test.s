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
	cs nopw 0x0(%rax,%rax,1)
	xchg   %ax,%ax

<not_zero>:
	xor    %eax,%eax
	xor    %ecx,%ecx
	cmp    %eax,%ecx
	sete   %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
