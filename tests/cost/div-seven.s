Disassembly of section .text:

<div_seven>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x7,%ecx
	cltd
	idiv   %ecx
	ret
