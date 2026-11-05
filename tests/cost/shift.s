Disassembly of section .text:

<shl>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	shl    %cl,%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	nopl   (%rax)

<sar>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	sar    %cl,%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	nopl   (%rax)

<shr>:
	mov    %edi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%eax
	mov    -0xc(%rsp),%ecx
	shr    %cl,%eax
	ret
