Disassembly of section .text:

<parameter>:
	mov    %edi,-0xc(%rsp)
	mov    %esi,-0x8(%rsp)
	mov    -0xc(%rsp),%ecx
	mov    -0x8(%rsp),%eax
	sub    %eax,%ecx
	mov    -0xc(%rsp),%eax
	add    %ecx,%eax
	ret
