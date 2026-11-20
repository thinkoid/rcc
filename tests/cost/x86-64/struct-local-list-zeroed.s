	.file	"struct-local-list-zeroed.c"
	.text
	.globl	point_fewer
	.type	point_fewer, @function
point_fewer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	leaq	-12(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	use@PLT
.Lpoint_fewer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	point_fewer, .-point_fewer
	.globl	wide_fewer
	.type	wide_fewer, @function
wide_fewer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, -32(%rbp)
	movq	$0, -24(%rbp)
	movq	$0, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	leaq	-32(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	use@PLT
.Lwide_fewer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	wide_fewer, .-wide_fewer
	.globl	large_fewer
	.type	large_fewer, @function
large_fewer:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$208, %rsp
	movq	$-200, %rax
.Llarge_fewer.2:
	movq	$0, (%rbp,%rax,1)
	addq	$8, %rax
	jne	.Llarge_fewer.2
	movq	$1, -200(%rbp)
	movq	$2, -192(%rbp)
	leaq	-200(%rbp), %rax
	movq	%rax, -208(%rbp)
	movq	-208(%rbp), %rdi
	call	use@PLT
.Llarge_fewer.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	large_fewer, .-large_fewer
	.globl	padded_full
	.type	padded_full, @function
padded_full:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movb	%dil, -1(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, -20(%rbp)
	movl	$0, -16(%rbp)
	movl	$0, -12(%rbp)
	movsbl	-1(%rbp), %eax
	movb	%al, -20(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -16(%rbp)
	movsbl	-1(%rbp), %eax
	movb	%al, -12(%rbp)
	leaq	-20(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	call	use@PLT
.Lpadded_full.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	padded_full, .-padded_full
	.globl	point_empty
	.type	point_empty, @function
point_empty:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lpoint_empty.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	point_empty, .-point_empty
	.globl	union_shorter
	.type	union_shorter, @function
union_shorter:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movw	$0, -4(%rbp)
	movw	$0, -2(%rbp)
	movb	$97, -4(%rbp)
	leaq	-4(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rdi
	call	use@PLT
.Lunion_shorter.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	union_shorter, .-union_shorter
	.section	.note.GNU-stack,"",@progbits
