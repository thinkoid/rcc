	.file	"g6-ptr-wrap.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	$-1, %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movl	-4(%rbp), %ecx
	addl	$2, %ecx
	shrq	%cl, %rax
	movq	%rax, -40(%rbp)
	movq	-24(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	-40(%rbp), %rdx
	leaq	(%rcx,%rdx,8), %rcx
	cmpq	%rcx, %rax
	setb	%al
	movzbl	%al, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
