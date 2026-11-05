Disassembly of section .text:

<sdiv>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cltd
	idiv   %ecx
	ret
	cs nopw 0x0(%rax,%rax,1)
	xchg   %ax,%ax

<srem>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cltd
	idiv   %ecx
	mov    %edx,%eax
	ret
