Disassembly of section .text:

<invariant>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    %edx,-0x10(%rsp)
	movl   $0x0,-0x14(%rsp)
	movl   $0x0,-0x18(%rsp)
	mov    -0x18(%rsp),%eax
	mov    -0x10(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <invariant+0x55>
	mov    -0x14(%rsp),%ecx
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%esi
	cltd
	idiv   %esi
	add    %ecx,%eax
	mov    %eax,-0x14(%rsp)
	mov    -0x18(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0x18(%rsp)
	jmp    <invariant+0x1c>
	mov    -0x14(%rsp),%eax
	ret
	nopw   0x0(%rax,%rax,1)

<invariant_constant>:
	mov    %edi,-0x8(%rsp)
	movl   $0x0,-0xc(%rsp)
	movl   $0x0,-0x10(%rsp)
	mov    -0x10(%rsp),%eax
	mov    -0x8(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <invariant_constant+0x4a>
	mov    -0xc(%rsp),%eax
	mov    $0x22,%ecx
	imul   $0xc,%ecx,%ecx
	add    %eax,%ecx
	mov    %ecx,-0xc(%rsp)
	mov    -0x10(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0x10(%rsp)
	jmp    <invariant_constant+0x14>
	mov    -0xc(%rsp),%eax
	ret
