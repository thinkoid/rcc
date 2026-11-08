	.file	"register-parameter.c"
	.text
	.globl	parameter
	.type	parameter, @function
parameter:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movl	%edi, %ebx
	movl	%esi, %r12d
	movl	%ebx, %eax
	subl	%r12d, %eax
	addl	%ebx, %eax
	jmp	.L1
.L1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	parameter, .-parameter
	.section	.note.GNU-stack,"",@progbits
