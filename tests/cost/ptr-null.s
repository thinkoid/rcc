Disassembly of section .text:

<test>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	cmp    $0x0,%rax
	je     <test+0x16>
	mov    $0x1,%eax
	ret
	xor    %eax,%eax
	ret
	nopl   0x0(%rax)

<equal>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	cmp    $0x0,%rax
	sete   %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
