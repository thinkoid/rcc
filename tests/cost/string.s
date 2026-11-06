Disassembly of section .text:

<second>:
	lea    0x0(%rip),%rax        # <second+0x7>
			R_X86_64_PC32	.data-0x4
	mov    %rax,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	mov    $0x1,%ecx
	movslq %ecx,%rcx
	movzbl (%rax,%rcx,1),%eax
	movsbl %al,%eax
	ret
