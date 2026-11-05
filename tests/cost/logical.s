Disassembly of section .text:

<both>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%ecx
	xor    %eax,%eax
	cmp    $0x0,%ecx
	je     <both+0x1d>
	mov    -0xc(%rsp),%eax
	cmp    $0x0,%eax
	setne  %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	nopl   (%rax)

<either>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%ecx
	mov    $0x1,%al
	cmp    $0x0,%ecx
	jne    <either+0x1d>
	mov    -0xc(%rsp),%eax
	cmp    $0x0,%eax
	setne  %al
	and    $0x1,%al
	movzbl %al,%eax
	ret
