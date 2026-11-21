	.file	"compatible-types.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	$12, %eax
	jmp	.Lf.1
.Lf.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	f, .-f
	.globl	pick
	.type	pick, @function
pick:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movl	$12, %eax
	jmp	.Lpick.1
.Lpick.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	through_long_long
	.type	through_long_long, @function
through_long_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	(%rax), %rax
	jmp	.Lthrough_long_long.1
.Lthrough_long_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through_long_long, .-through_long_long
	.globl	through_shape
	.type	through_shape, @function
through_shape:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	(%rax), %eax
	jmp	.Lthrough_shape.1
.Lthrough_shape.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through_shape, .-through_shape
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	$5, -8(%rbp)
	movl	$1, -12(%rbp)
	movl	$1, count(%rip)
	movl	$1, total(%rip)
	leaq	grid(%rip), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	f@PLT
	cmpl	$12, %eax
	je	.Lmain.2
	movl	$101, %eax
	jmp	.Lmain.1
.Lmain.2:
	movl	$0, -28(%rbp)
	movl	-28(%rbp), %edi
	call	pick@PLT
	cmpl	$12, %eax
	je	.Lmain.3
	movl	$102, %eax
	jmp	.Lmain.1
.Lmain.3:
	leaq	-8(%rbp), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rdi
	call	through_long_long@PLT
	cmpq	$5, %rax
	je	.Lmain.4
	movl	$103, %eax
	jmp	.Lmain.1
.Lmain.4:
	leaq	-12(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rdi
	call	through_shape@PLT
	cmpl	$1, %eax
	je	.Lmain.5
	movl	$104, %eax
	jmp	.Lmain.1
.Lmain.5:
	cmpl	$1, count(%rip)
	je	.Lmain.6
	movl	$105, %eax
	jmp	.Lmain.1
.Lmain.6:
	cmpl	$1, total(%rip)
	je	.Lmain.7
	movl	$106, %eax
	jmp	.Lmain.1
.Lmain.7:
	movl	$42, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
	.globl	rows
	.type	rows, @object
	.size	rows, 8
	.p2align 3
rows:
	.quad	grid
	.bss
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.zero	4
	.globl	total
	.type	total, @object
	.size	total, 4
	.p2align 2
total:
	.zero	4
	.globl	grid
	.type	grid, @object
	.size	grid, 12
	.p2align 2
grid:
	.zero	12
	.globl	any
	.type	any, @object
	.size	any, 8
	.p2align 3
any:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
