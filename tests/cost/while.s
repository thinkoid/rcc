Disassembly of section .text:

<count>:
	mov    %edi,-0x8(%rsp)
	movl   $0x0,-0xc(%rsp)
	mov    -0xc(%rsp),%eax
	mov    -0x8(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <count+0x30>
	mov    -0xc(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0xc(%rsp)
	jmp    <count+0xc>
	mov    -0xc(%rsp),%eax
	ret
