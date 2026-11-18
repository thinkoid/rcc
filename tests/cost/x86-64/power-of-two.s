	.file	"power-of-two.c"
	.text
	.globl	times_four
	.type	times_four, @function
times_four:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	imull	$4, -4(%rbp), %eax
	jmp	.Ltimes_four.1
.Ltimes_four.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	times_four, .-times_four
	.globl	quarter
	.type	quarter, @function
quarter:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	xorl	%edx, %edx
	divl	%ecx
	jmp	.Lquarter.1
.Lquarter.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	quarter, .-quarter
	.globl	quarter_rest
	.type	quarter_rest, @function
quarter_rest:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	$4, %ecx
	xorl	%edx, %edx
	divl	%ecx
	movl	%edx, %eax
	jmp	.Lquarter_rest.1
.Lquarter_rest.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	quarter_rest, .-quarter_rest
	.section	.note.GNU-stack,"",@progbits
