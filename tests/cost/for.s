Disassembly of section .text:

<sum>:
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
	je     <sum+0x46>
	mov    -0xc(%rsp),%eax
	mov    -0x10(%rsp),%ecx
	add    %eax,%ecx
	mov    %ecx,-0xc(%rsp)
	mov    -0x10(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0x10(%rsp)
	jmp    <sum+0x14>
	mov    -0xc(%rsp),%eax
	ret
