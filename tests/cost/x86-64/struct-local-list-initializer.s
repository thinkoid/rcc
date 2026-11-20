	.file	"struct-local-list-initializer.c"
	.text
	.globl	full
	.type	full, @function
full:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	movl	$1, -8(%rbp)
	movl	$2, -4(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lfull.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	full, .-full
	.globl	nested
	.type	nested, @function
nested:
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
	movl	$1, -16(%rbp)
	movl	$2, -12(%rbp)
	movl	$3, -8(%rbp)
	movl	$4, -4(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	use@PLT
.Lnested.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	arrays
	.type	arrays, @function
arrays:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	$0, -16(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	movb	$97, -16(%rbp)
	movl	$1, -12(%rbp)
	movl	$2, -8(%rbp)
	movl	$0, -32(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, -24(%rbp)
	movl	$0, -20(%rbp)
	movb	$98, -32(%rbp)
	movl	$3, -28(%rbp)
	movl	$4, -24(%rbp)
	movl	$5, -20(%rbp)
	leaq	-16(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	use@PLT
	leaq	-32(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	use@PLT
.Larrays.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	arrays, .-arrays
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movw	$0, -4(%rbp)
	movw	$0, -2(%rbp)
	movb	$120, -4(%rbp)
	movb	$121, -3(%rbp)
	movb	$122, -2(%rbp)
	leaq	-4(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lfirst.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	first, .-first
	.globl	called
	.type	called, @function
called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	call	f@PLT
	movl	%eax, -8(%rbp)
	call	g@PLT
	movl	%eax, -4(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lcalled.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	counted
	.type	counted, @function
counted:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$0, -4(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	$3, -8(%rbp)
	.p2align 5
.Lcounted.2:
	movl	-12(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	.Lcounted.4
	movl	-12(%rbp), %eax
	addl	%eax, -4(%rbp)
	addl	$1, -12(%rbp)
	jmp	.Lcounted.2
.Lcounted.4:
	movl	-4(%rbp), %eax
	jmp	.Lcounted.1
.Lcounted.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	counted, .-counted
	.section	.note.GNU-stack,"",@progbits
