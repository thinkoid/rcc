Disassembly of section .text:

<div_busy>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    %edx,-0x10(%rsp)
	mov    -0x8(%rsp),%ecx
	mov    -0xc(%rsp),%eax
	mov    -0x10(%rsp),%esi
	cltd
	idiv   %esi
	add    %ecx,%eax
	ret
	xchg   %ax,%ax

<rem_busy>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    %edx,-0x10(%rsp)
	mov    -0x8(%rsp),%ecx
	mov    -0xc(%rsp),%eax
	mov    -0x10(%rsp),%esi
	cltd
	idiv   %esi
	add    %edx,%ecx
	mov    %ecx,%eax
	ret
