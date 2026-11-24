	.file	"bool-compound-volatile-address.c"
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
	movl	%edi, -4(%rbp)
	movq	p(%rip), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	-16(%rbp), %rcx
	movzbl	(%rcx), %ecx
	orl	-4(%rbp), %ecx
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
	.bss
	.globl	p
	.type	p, @object
	.size	p, 8
	.p2align 3
p:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
