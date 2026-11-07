Disassembly of section .text:

<same>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	cmp    %eax,%eax
	sete   %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
