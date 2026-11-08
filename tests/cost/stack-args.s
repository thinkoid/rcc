	.file	"stack-args.c"
	.text
	.globl	seven
	.type	seven, @function
seven:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$1, %eax
	movl	%eax, -4(%rbp)
	movl	$2, %eax
	movl	%eax, -8(%rbp)
	movl	$3, %eax
	movl	%eax, -12(%rbp)
	movl	$4, %eax
	movl	%eax, -16(%rbp)
	movl	$5, %eax
	movl	%eax, -20(%rbp)
	movl	$6, %eax
	movl	%eax, -24(%rbp)
	movl	$7, %eax
	movl	%eax, -28(%rbp)
	subq	$8, %rsp
	movl	-28(%rbp), %eax
	pushq	%rax
	movl	-4(%rbp), %edi
	movl	-8(%rbp), %esi
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %ecx
	movl	-20(%rbp), %r8d
	movl	-24(%rbp), %r9d
	call	f7@PLT
	addq	$16, %rsp
	jmp	.Lseven.1
.Lseven.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	seven, .-seven
	.globl	eight
	.type	eight, @function
eight:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	$1, %eax
	movl	%eax, -4(%rbp)
	movl	$2, %eax
	movl	%eax, -8(%rbp)
	movl	$3, %eax
	movl	%eax, -12(%rbp)
	movl	$4, %eax
	movl	%eax, -16(%rbp)
	movl	$5, %eax
	movl	%eax, -20(%rbp)
	movl	$6, %eax
	movl	%eax, -24(%rbp)
	movl	$7, %eax
	movl	%eax, -28(%rbp)
	movl	$8, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	pushq	%rax
	movl	-28(%rbp), %eax
	pushq	%rax
	movl	-4(%rbp), %edi
	movl	-8(%rbp), %esi
	movl	-12(%rbp), %edx
	movl	-16(%rbp), %ecx
	movl	-20(%rbp), %r8d
	movl	-24(%rbp), %r9d
	call	f8@PLT
	addq	$16, %rsp
	jmp	.Leight.1
.Leight.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	eight, .-eight
	.section	.note.GNU-stack,"",@progbits
