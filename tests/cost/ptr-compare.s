Disassembly of section .text:

<less>:
	mov    %rdi,-0x10(%rsp)
	mov    %rsi,-0x18(%rsp)
	mov    -0x10(%rsp),%rax
	mov    -0x18(%rsp),%rcx
	cmp    %rcx,%rax
	setb   %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
