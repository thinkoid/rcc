Disassembly of section .text:

<subscript>:
	mov    %edi,-0x1c(%rsp)
	mov    %esi,-0x18(%rsp)
	mov    -0x1c(%rsp),%eax
	cltq
	mov    -0x18(%rsp),%ecx
	mov    %ecx,-0x14(%rsp,%rax,4)
	mov    -0x1c(%rsp),%eax
	cltq
	mov    -0x14(%rsp,%rax,4),%eax
	ret
