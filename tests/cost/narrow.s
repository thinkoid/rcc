Disassembly of section .text:

<f>:
	mov    %dil,-0x5(%rsp)
	mov    %si,-0x4(%rsp)
	movzbl -0x5(%rsp),%eax
	movsbl %al,%eax
	mov    $0x4,%ecx
	cltd
	idiv   %ecx
	movzwl -0x4(%rsp),%ecx
	movswl %cx,%ecx
	add    %eax,%ecx
	movsbl %cl,%eax
	ret
	nopl   0x0(%rax,%rax,1)

<g>:
	mov    %dil,-0x2(%rsp)
	movzbl -0x2(%rsp),%eax
	movzbl %al,%eax
	add    $0x1,%eax
	movzbl %al,%eax
	ret
	cs nopw 0x0(%rax,%rax,1)
	xchg   %ax,%ax

<h>:
	push   %rax
	mov    %edi,(%rsp)
	mov    (%rsp),%eax
	movsbl %al,%edi
	mov    (%rsp),%eax
	movswl %ax,%esi
	call   <h+0x15>
			R_X86_64_PLT32	f-0x4
	movsbl %al,%eax
	pop    %rcx
	ret
