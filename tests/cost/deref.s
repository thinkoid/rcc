Disassembly of section .text:

<load>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	mov    (%rax),%eax
	ret
	nopl   (%rax)

<store>:
	mov    %rdi,-0x8(%rsp)
	mov    %esi,-0xc(%rsp)
	mov    -0x8(%rsp),%rax
	mov    -0xc(%rsp),%ecx
	mov    %ecx,(%rax)
	ret
