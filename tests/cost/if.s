Disassembly of section .text:

<choose>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <choose+0x29>
	mov    -0x8(%rsp),%eax
	mov    %eax,-0x10(%rsp)
	jmp    <choose+0x31>
	mov    -0xc(%rsp),%eax
	mov    %eax,-0x10(%rsp)
	mov    -0x10(%rsp),%eax
	ret
