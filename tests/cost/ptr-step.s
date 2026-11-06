Disassembly of section .text:

<step>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	add    $0x4,%rax
	mov    %rax,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	mov    $0x2,%ecx
	movslq %ecx,%rcx
	xor    %edx,%edx
	sub    %rcx,%rdx
	shl    $0x2,%rdx
	add    %rax,%rdx
	mov    %rdx,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret
