Disassembly of section .text:

<empty_if>:
	mov    %edi,-0x8(%rsp)
	movl   $0x0,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	cmp    $0x0,%eax
	je     <empty_if+0x17>
	jmp    <empty_if+0x17>
	mov    -0x8(%rsp),%eax
	cmp    $0x1,%eax
	setg   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <empty_if+0x33>
	movl   $0x1,-0xc(%rsp)
	mov    -0xc(%rsp),%eax
	ret
