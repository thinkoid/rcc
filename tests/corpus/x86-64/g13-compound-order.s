	.file	"g13-compound-order.c"
	.text
	.globl	bump
	.type	bump, @function
bump:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$10, (%rax)
	movl	$1, %eax
	jmp	.Lbump.1
.Lbump.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	bump, .-bump
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$56, %rsp
	movl	$1, -12(%rbp)
	movl	$1, -16(%rbp)
	movl	$1, -20(%rbp)
	movl	$1, -24(%rbp)
	movl	$1, %ebx
	leaq	-12(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	bump@PLT
	addl	%eax, -12(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	bump@PLT
	movl	-16(%rbp), %ecx
	addl	%eax, %ecx
	movl	%ecx, -16(%rbp)
	movl	%ecx, -28(%rbp)
	leaq	-20(%rbp), %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdi
	call	bump@PLT
	movl	-20(%rbp), %ecx
	imull	%eax, %ecx
	movl	%ecx, -20(%rbp)
	movl	$2, %eax
	movl	%eax, -24(%rbp)
	addl	%eax, -24(%rbp)
	movl	$2, %ebx
	addl	%ebx, %ebx
	movl	-12(%rbp), %eax
	cmpl	$11, %eax
	sete	%al
	movzbl	%al, %eax
	movl	-16(%rbp), %ecx
	cmpl	$11, %ecx
	sete	%cl
	movzbl	%cl, %ecx
	imull	$2, %ecx, %ecx
	addl	%ecx, %eax
	movl	-28(%rbp), %ecx
	cmpl	$11, %ecx
	sete	%cl
	movzbl	%cl, %ecx
	imull	$4, %ecx, %ecx
	addl	%ecx, %eax
	movl	-20(%rbp), %ecx
	cmpl	$10, %ecx
	sete	%cl
	movzbl	%cl, %ecx
	imull	$8, %ecx, %ecx
	addl	%ecx, %eax
	movl	-24(%rbp), %ecx
	cmpl	$4, %ecx
	sete	%cl
	movzbl	%cl, %ecx
	imull	$16, %ecx, %ecx
	addl	%ecx, %eax
	cmpl	$4, %ebx
	sete	%cl
	movzbl	%cl, %ecx
	imull	$32, %ecx, %ecx
	addl	%ecx, %eax
	jmp	.Lmain.1
.Lmain.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
