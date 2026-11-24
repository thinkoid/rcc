	.file	"bool-compound-side-effect.c"
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
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rax
	movq	%rax, -8(%rbp)
	movq	%rcx, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	-16(%rbp), %rcx
	movzbl	(%rcx), %ecx
	orl	$1, %ecx
	cmpl	$0, %ecx
	setne	%cl
	movzbl	%cl, %ecx
	movb	%cl, (%rax)
.Lmark.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	mark, .-mark
	.section	.note.GNU-stack,"",@progbits
