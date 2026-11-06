Disassembly of section .text:

<diff>:
	mov    %rdi,-0x10(%rsp)
	mov    %rsi,-0x18(%rsp)
	mov    -0x10(%rsp),%rax
	mov    -0x18(%rsp),%rcx
	sub    %rcx,%rax
	ret
