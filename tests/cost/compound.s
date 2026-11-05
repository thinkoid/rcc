Disassembly of section .text:

<compound>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    %eax,-0x10(%rsp)
	mov    -0x10(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	add    %eax,%ecx
	mov    %ecx,-0x10(%rsp)
	mov    -0x10(%rsp),%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	nopl   (%rax)

<narrow>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    %al,-0xd(%rsp)
	movzbl -0xd(%rsp),%eax
	movsbl %al,%eax
	mov    -0xc(%rsp),%ecx
	add    %eax,%ecx
	mov    %cl,-0xd(%rsp)
	movzbl -0xd(%rsp),%eax
	movsbl %al,%eax
	ret
