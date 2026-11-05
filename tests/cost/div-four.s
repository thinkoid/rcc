Disassembly of section .text:

<div_four>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x4,%ecx
	cltd
	idiv   %ecx
	ret
