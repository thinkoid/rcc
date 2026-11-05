Disassembly of section .text:

<join>:
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
	je     <join+0x4a>
	mov    -0x8(%rsp),%eax
	cmp    $0x0,%eax
	je     <join+0x3d>
	mov    -0x10(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0x10(%rsp)
	jmp    <join+0x48>
	mov    -0x10(%rsp),%eax
	add    $0x2,%eax
	mov    %eax,-0x10(%rsp)
	jmp    <join+0x10>
	mov    -0x10(%rsp),%eax
	ret
