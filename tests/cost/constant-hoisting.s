Disassembly of section .text:

<two_constants>:
	mov    %rdi,-0x10(%rsp)
	mov    %rsi,-0x18(%rsp)
	mov    -0x10(%rsp),%rax
	movabs $0x123456789abcdef,%rcx
	add    %rax,%rcx
	mov    -0x18(%rsp),%rdx
	add    %rcx,%rdx
	movabs $0x123456789abcdf7,%rax
	add    %rdx,%rax
	ret
