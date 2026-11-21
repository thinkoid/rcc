	.file	"struct-local-list-string.c"
	.text
	.globl	exact
	.type	exact, @function
exact:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	movzbl	.Ls0(%rip), %eax
	movb	%al, -8(%rbp)
	movzbl	.Ls0+1(%rip), %eax
	movb	%al, -7(%rbp)
	movzbl	.Ls0+2(%rip), %eax
	movb	%al, -6(%rbp)
	movzbl	.Ls0+3(%rip), %eax
	movb	%al, -5(%rbp)
	movl	$1, -4(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lexact.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	exact, .-exact
	.globl	longer
	.type	longer, @function
longer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$0, -16(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	movzbl	.Ls1(%rip), %eax
	movb	%al, -16(%rbp)
	movzbl	.Ls1+1(%rip), %eax
	movb	%al, -15(%rbp)
	movzbl	.Ls1+2(%rip), %eax
	movb	%al, -14(%rbp)
	movzbl	.Ls1+3(%rip), %eax
	movb	%al, -13(%rbp)
	movl	$1, -4(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	use@PLT
.Llonger.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	longer, .-longer
	.data
.Ls0:
	.string	"abc"
.Ls1:
	.string	"abc"
	.section	.note.GNU-stack,"",@progbits
