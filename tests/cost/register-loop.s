Disassembly of section .text:

<sum>:
	xor    %eax,%eax
	xor    %ecx,%ecx
	cmp    %edi,%ecx
	setl   %dl
	and    $0x1,%dl
	movzbl %dl,%edx
	cmp    $0x0,%edx
	je     <sum+0x1b>
	add    %ecx,%eax
	add    $0x1,%ecx
	jmp    <sum+0x4>
	ret
