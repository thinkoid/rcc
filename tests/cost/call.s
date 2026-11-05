Disassembly of section .text:

<call>:
	sub    $0x18,%rsp
	mov    %edi,0x10(%rsp)
	mov    %esi,0xc(%rsp)
	mov    0xc(%rsp),%edi
	mov    0x10(%rsp),%esi
	call   <call+0x19>
			R_X86_64_PLT32	f-0x4
	add    $0x18,%rsp
	ret
