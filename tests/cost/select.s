Disassembly of section .text:

<select>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <select+0x25>
	mov    -0x8(%rsp),%eax
	jmp    <select+0x29>
	mov    -0xc(%rsp),%eax
	ret
