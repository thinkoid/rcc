	.file	"twice.c"
	.text
	.globl	twice
	.type	twice, @function
twice:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %ecx
	imull	%ecx, %eax
	movl	-4(%rbp), %ecx
	movl	-8(%rbp), %edx
	imull	%edx, %ecx
	addl	%ecx, %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	twice, .-twice
	.globl	twice_constant
	.type	twice_constant, @function
twice_constant:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	addl	$408, %eax
	addl	$408, %eax
	jmp	.L2
.L2:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	twice_constant, .-twice_constant
	.section	.note.GNU-stack,"",@progbits
