Disassembly of section .text:

<row>:
	mov    %edi,-0x2c(%rsp)
	mov    -0x2c(%rsp),%eax
	cltq
	lea    -0x20(%rsp),%rcx
	imul   $0xc,%rax,%rax
	add    %rcx,%rax
	mov    %rax,-0x28(%rsp)
	mov    -0x28(%rsp),%rax
	ret
