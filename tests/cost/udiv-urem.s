Disassembly of section .text:

<udiv>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	xor    %edx,%edx
	div    %ecx
	ret
	cs nopw 0x0(%rax,%rax,1)
	nop

<urem>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	xor    %edx,%edx
	div    %ecx
	mov    %edx,%eax
	ret
