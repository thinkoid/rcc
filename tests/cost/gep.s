Disassembly of section .text:

<gep>:
	mov    %edi,-0x24(%rsp)
	mov    -0x24(%rsp),%eax
	cltq
	lea    -0x18(%rsp),%rcx
	shl    $0x2,%rax
	add    %rcx,%rax
	mov    %rax,-0x20(%rsp)
	mov    -0x20(%rsp),%rax
	ret
