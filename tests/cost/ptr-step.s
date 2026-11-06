Disassembly of section .text:

<step>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	add    $0x4,%rax
	mov    %rax,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	xor    %ecx,%ecx
	sub    $0x2,%rcx
	shl    $0x2,%rcx
	add    %rax,%rcx
	mov    %rcx,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret
