Disassembly of section .text:

<block_order>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	movl   $0x0,-0x10(%rsp)
	mov    -0x10(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <block_order+0x51>
	mov    -0x10(%rsp),%eax
	mov    -0x8(%rsp),%ecx
	cmp    %ecx,%eax
	sete   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <block_order+0x44>
	mov    $0x1,%eax
	ret
	mov    -0x10(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0x10(%rsp)
	jmp    <block_order+0x10>
	xor    %eax,%eax
	ret
