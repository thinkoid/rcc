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
	cmpl	(%rcx), %eax
	setg	%al
	movzbl	%al, %eax
	movq	-24(%rbp), %rcx
	movl	(%rcx), %ecx
	movq	-32(%rbp), %rdx
	cmpl	(%rdx), %ecx
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
	movl	$42, -24(%rbp)
	movl	$-7, -20(%rbp)
	movl	$1000, -16(%rbp)
	movl	$0, -12(%rbp)
	movl	$-7, -8(%rbp)
	movl	$3, -4(%rbp)
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
	cmpl	$6, -28(%rbp)
	jge	.Lmain.4
	movl	-28(%rbp), %eax
	subl	$1, %eax
	movslq	%eax, %rax
	movl	-24(%rbp,%rax,4), %eax
	movslq	-28(%rbp), %rcx
	cmpl	-24(%rbp,%rcx,4), %eax
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
