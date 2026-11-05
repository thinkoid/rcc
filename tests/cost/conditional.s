Disassembly of section .text:

<conditional>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    %edx,-0x10(%rsp)
	mov    -0x8(%rsp),%eax
	cmp    $0x0,%eax
	je     <conditional+0x1b>
	mov    -0xc(%rsp),%eax
	jmp    <conditional+0x1f>
	mov    -0x10(%rsp),%eax
	ret
