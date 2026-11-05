Disassembly of section .text:

<twice>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	imul   %eax,%ecx
	mov    -0x8(%rsp),%edx
	mov    -0xc(%rsp),%eax
	imul   %edx,%eax
	add    %ecx,%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	nopl   0x0(%rax,%rax,1)

<twice_constant>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    $0x22,%ecx
	imul   $0xc,%ecx,%ecx
	add    %eax,%ecx
	mov    $0x22,%eax
	imul   $0xc,%eax,%eax
	add    %ecx,%eax
	ret
