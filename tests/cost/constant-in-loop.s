Disassembly of section .text:

<constant_in_loop>:
	mov    %rdi,-0x18(%rsp)
	mov    %esi,-0xc(%rsp)
	movl   $0x0,-0x1c(%rsp)
	mov    -0x1c(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	cmp    %ecx,%eax
	setl   %al
	and    $0x1,%al
	movzbl %al,%eax
	cmp    $0x0,%eax
	je     <constant_in_loop+0x4c>
	mov    -0x18(%rsp),%rax
	movabs $0x123456789abcdef,%rcx
	add    %rax,%rcx
	mov    %rcx,-0x18(%rsp)
	mov    -0x1c(%rsp),%eax
	add    $0x1,%eax
	mov    %eax,-0x1c(%rsp)
	jmp    <constant_in_loop+0x11>
	mov    -0x18(%rsp),%rax
	ret
