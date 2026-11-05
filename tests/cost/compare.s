Disassembly of section .text:

<less>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
	nopl   0x0(%rax,%rax,1)

<equal>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cmp    %ecx,%eax
	sete   %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
