	.file	"header-qsort.c"
	.text
	.globl	compare
	.type	compare, @function
compare:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rax
	movl	(%rax), %eax
	movq	-32(%rbp), %rcx
	movl	(%rcx), %ecx
	cmpl	%ecx, %eax
	setg	%al
	movzbl	%al, %eax
	movq	-24(%rbp), %rcx
	movl	(%rcx), %ecx
	movq	-32(%rbp), %rdx
	movl	(%rdx), %edx
	cmpl	%edx, %ecx
	setl	%cl
	movzbl	%cl, %ecx
	subl	%ecx, %eax
	jmp	.Lcompare.1
.Lcompare.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	compare, .-compare
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	leaq	-24(%rbp), %rax
	movl	$42, (%rax)
	leaq	-24(%rbp), %rax
	movl	$-7, 4(%rax)
	leaq	-24(%rbp), %rax
	movl	$1000, 8(%rax)
	leaq	-24(%rbp), %rax
	movl	$0, 12(%rax)
	leaq	-24(%rbp), %rax
	movl	$-7, 16(%rax)
	leaq	-24(%rbp), %rax
	movl	$3, 20(%rax)
	leaq	-24(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	$6, -48(%rbp)
	movq	$4, -56(%rbp)
	leaq	compare(%rip), %rax
	movq	%rax, -64(%rbp)
	movq	-40(%rbp), %rdi
	movq	-48(%rbp), %rsi
	movq	-56(%rbp), %rdx
	movq	-64(%rbp), %rcx
	call	qsort@PLT
	movl	$1, -28(%rbp)
	.p2align 5
.Lmain.2:
	movl	-28(%rbp), %eax
	cmpl	$6, %eax
	jge	.Lmain.4
	leaq	-24(%rbp), %rax
	movl	-28(%rbp), %ecx
	subl	$1, %ecx
	movslq	%ecx, %rcx
	movl	(%rax,%rcx,4), %eax
	leaq	-24(%rbp), %rcx
	movl	-28(%rbp), %edx
	movslq	%edx, %rdx
	movl	(%rcx,%rdx,4), %ecx
	cmpl	%ecx, %eax
	jle	.Lmain.5
	movl	$1, %eax
	jmp	.Lmain.1
.Lmain.5:
.Lmain.3:
	addl	$1, -28(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	movl	$0, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
