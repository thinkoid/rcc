Disassembly of section .text:

<signed_>:
	mov    %edi,-0x14(%rsp)
	mov    -0x14(%rsp),%eax
	mov    %al,-0x19(%rsp)
	movzbl -0x19(%rsp),%eax
	movsbl %al,%eax
	mov    %eax,-0x18(%rsp)
	mov    -0x18(%rsp),%eax
	cltq
	mov    %rax,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret
	nopl   0x0(%rax)

<unsigned_>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    %al,-0xd(%rsp)
	movzbl -0xd(%rsp),%eax
	movzbl %al,%eax
	mov    %eax,-0xc(%rsp)
	mov    -0xc(%rsp),%eax
	ret
	nopl   (%rax)

<same>:
	mov    %edi,-0x8(%rsp)
	mov    -0x8(%rsp),%eax
	mov    %eax,-0xc(%rsp)
	mov    -0xc(%rsp),%eax
	ret
