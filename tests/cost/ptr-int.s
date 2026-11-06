Disassembly of section .text:

<to_long>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret
	nopl   0x0(%rax,%rax,1)

<from_long>:
	mov    %rdi,-0x10(%rsp)
	mov    -0x10(%rsp),%rax
	ret
	nopl   0x0(%rax,%rax,1)

<from_int>:
	mov    %edi,-0xc(%rsp)
	mov    -0xc(%rsp),%eax
	cltq
	ret
