	.file	"parameter.c"
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
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	subl	-8(%rbp), %eax
	addl	-4(%rbp), %eax
	jmp	.Lparameter.1
.Lparameter.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	parameter, .-parameter
	.section	.note.GNU-stack,"",@progbits
