Disassembly of section .text:

<add>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%ecx
	mov    -0xc(%rsp),%eax
	add    %ecx,%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	nopl   (%rax)

<sub>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	sub    %ecx,%eax
	ret
