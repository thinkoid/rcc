Disassembly of section .text:

<second>:
	lea    0x0(%rip),%rax        # <second+0x7>
			R_X86_64_PC32	.data-0x4
	mov    %rax,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	movzbl 0x1(%rax),%eax
	movsbl %al,%eax
	ret
