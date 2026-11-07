Disassembly of section .text:

<which>:
	mov    -0x8(%rsp),%eax
	cmp    $0xa,%eax
	setl   %cl
	and    $0x1,%cl
	movzbl %cl,%ecx
	cmp    $0x0,%ecx
	je     <which+0x1b>
	mov    $0x1,%eax
	ret
	cmp    $0xa,%eax
	setge  %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <which+0x31>
	mov    $0x2,%eax
	ret
	mov    $0x3,%eax
	ret
