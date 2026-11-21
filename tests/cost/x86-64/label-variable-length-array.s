	.file	"label-variable-length-array.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
.Lf.2:
	addl	$1, -8(%rbp)
	movslq	-4(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	%rsp, -24(%rbp)
	movq	-16(%rbp), %rax
	subq	%rax, %rsp
	andq	$-16, %rsp
	movq	%rsp, %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movb	$1, (%rax)
	cmpl	$2, -8(%rbp)
	jge	.Lf.3
	movq	-24(%rbp), %rsp
	jmp	.Lf.2
.Lf.3:
	movq	-32(%rbp), %rax
	movsbl	(%rax), %eax
	jmp	.Lf.1
.Lf.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	f, .-f
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$1, -4(%rbp)
	movl	-4(%rbp), %edi
	call	f@PLT
	cmpl	$1, %eax
	setne	%al
	movzbl	%al, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
