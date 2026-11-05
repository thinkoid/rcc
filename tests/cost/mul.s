Disassembly of section .text:

<mul>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%ecx
	mov    -0xc(%rsp),%eax
	imul   %ecx,%eax
	ret
