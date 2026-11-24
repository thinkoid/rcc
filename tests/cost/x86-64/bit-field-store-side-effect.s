	.file	"bit-field-store-side-effect.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, -12(%rbp)
	movslq	%edx, %rdx
	orb	$1, 4(%rax,%rdx,8)
.Lmark.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mark, .-mark
	.globl	raise
	.type	raise, @function
raise:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %edx
	addl	$1, %ecx
	movl	%ecx, -12(%rbp)
	movslq	%edx, %rdx
	leaq	(%rax,%rdx,8), %rax
	addq	$4, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movl	-16(%rbp), %ecx
	shll	$1, %ecx
	andl	$14, %ecx
	movq	-24(%rbp), %rdx
	movzbl	(%rdx), %edx
	andl	$-15, %edx
	orl	%edx, %ecx
	movb	%cl, (%rax)
.Lraise.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	raise, .-raise
	.section	.note.GNU-stack,"",@progbits
