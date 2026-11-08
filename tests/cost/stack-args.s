Disassembly of section .text:

<seven>:
	sub    $0x18,%rsp
	mov    $0x1,%edi
	mov    $0x2,%esi
	mov    $0x3,%edx
	mov    $0x4,%ecx
	mov    $0x5,%r8d
	mov    $0x6,%r9d
	movl   $0x7,(%rsp)
	call   <seven+0x30>
			R_X86_64_PLT32	f7-0x4
	add    $0x18,%rsp
	ret
	cs nopw 0x0(%rax,%rax,1)
	nop

<eight>:
	push   %rax
	mov    $0x1,%edi
	mov    $0x2,%esi
	mov    $0x3,%edx
	mov    $0x4,%ecx
	mov    $0x5,%r8d
	mov    $0x6,%r9d
	push   $0x8
	push   $0x7
	call   <eight+0x2a>
			R_X86_64_PLT32	f8-0x4
	add    $0x10,%rsp
	pop    %rcx
	ret
