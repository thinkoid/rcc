	.file	"call-pointer-stack.c"
	.text
	.globl	nine
	.type	nine, @function
nine:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movl	$1, -20(%rbp)
	movl	$2, -24(%rbp)
	movl	$3, -28(%rbp)
	movl	$4, -32(%rbp)
	movl	$5, -36(%rbp)
	movl	$6, -40(%rbp)
	movl	$7, -44(%rbp)
	movl	$8, -48(%rbp)
	movl	$9, -52(%rbp)
	subq	$8, %rsp
	movl	-52(%rbp), %eax
	pushq	%rax
	movl	-48(%rbp), %eax
	pushq	%rax
	movl	-44(%rbp), %eax
	pushq	%rax
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-28(%rbp), %edx
	movl	-32(%rbp), %ecx
	movl	-36(%rbp), %r8d
	movl	-40(%rbp), %r9d
	call	*-16(%rbp)
	addq	$32, %rsp
	jmp	.Lnine.1
.Lnine.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nine, .-nine
	.section	.note.GNU-stack,"",@progbits
