Disassembly of section .text:

<spill>:
	push   %rbp
	push   %rbx
	mov    %edi,-0xc(%rsp)
	mov    %esi,-0x10(%rsp)
	mov    %edx,-0x14(%rsp)
	mov    %ecx,-0x18(%rsp)
	mov    %r8d,-0x1c(%rsp)
	mov    %r9d,-0x8(%rsp)
	mov    -0xc(%rsp),%eax
	mov    -0x10(%rsp),%ecx
	mov    -0x14(%rsp),%edx
	mov    -0x18(%rsp),%esi
	mov    -0x1c(%rsp),%edi
	mov    -0x8(%rsp),%r8d
	mov    -0xc(%rsp),%r9d
	mov    -0x10(%rsp),%r10d
	mov    -0x14(%rsp),%r11d
	mov    -0x18(%rsp),%ebx
	mov    -0x1c(%rsp),%ebp
	sub    %ebp,%ebx
	sub    %ebx,%r11d
	sub    %r11d,%r10d
	sub    %r10d,%r9d
	sub    %r9d,%r8d
	sub    %r8d,%edi
	sub    %edi,%esi
	sub    %esi,%edx
	sub    %edx,%ecx
	sub    %ecx,%eax
	pop    %rbx
	pop    %rbp
	ret
