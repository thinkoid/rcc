Disassembly of section .text:

<mul_three>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	imul   $0x3,%eax,%eax
	ret
