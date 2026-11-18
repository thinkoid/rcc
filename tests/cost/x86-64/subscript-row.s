	.file	"subscript-row.c"
	.text
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	-24(%rbp), %rax
	jmp	.Lfirst.1
.Lfirst.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	first, .-first
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	-20(%rbp), %eax
	jmp	.Lelement.1
.Lelement.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	second
	.type	second, @function
second:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	-12(%rbp), %rax
	jmp	.Lsecond.1
.Lsecond.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	second, .-second
	.globl	through
	.type	through, @function
through:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	12(%rax), %rax
	jmp	.Lthrough.1
.Lthrough.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through, .-through
	.globl	beyond
	.type	beyond, @function
beyond:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movabsq	$2400000000, %rcx
	addq	%rcx, %rax
	jmp	.Lbeyond.1
.Lbeyond.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	beyond, .-beyond
	.globl	column
	.type	column, @function
column:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	movl	12(%rax,%rcx,4), %eax
	jmp	.Lcolumn.1
.Lcolumn.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column, .-column
	.globl	column_first
	.type	column_first, @function
column_first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movslq	-12(%rbp), %rax
	movq	-8(%rbp), %rcx
	movl	12(%rcx,%rax,4), %eax
	jmp	.Lcolumn_first.1
.Lcolumn_first.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column_first, .-column_first
	.globl	column_beyond
	.type	column_beyond, @function
column_beyond:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movabsq	$2400000000, %rcx
	addq	%rcx, %rax
	movslq	-12(%rbp), %rcx
	movl	(%rax,%rcx,4), %eax
	jmp	.Lcolumn_beyond.1
.Lcolumn_beyond.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column_beyond, .-column_beyond
	.globl	column_sum
	.type	column_sum, @function
column_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	leaq	12(%rax), %rax
	movslq	-12(%rbp), %rcx
	movl	2147483644(%rax,%rcx,4), %eax
	jmp	.Lcolumn_sum.1
.Lcolumn_sum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column_sum, .-column_sum
	.globl	column_wide
	.type	column_wide, @function
column_wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	leaq	24(%rax), %rax
	movslq	-12(%rbp), %rcx
	imulq	$12, %rcx, %rcx
	addq	%rcx, %rax
	jmp	.Lcolumn_wide.1
.Lcolumn_wide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column_wide, .-column_wide
	.globl	row_sum
	.type	row_sum, @function
row_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	movslq	-12(%rbp), %rcx
	leaq	12(%rax,%rcx,4), %rax
	jmp	.Lrow_sum.1
.Lrow_sum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_sum, .-row_sum
	.globl	row_sum_first
	.type	row_sum_first, @function
row_sum_first:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movslq	-12(%rbp), %rax
	movq	-8(%rbp), %rcx
	leaq	12(%rcx,%rax,4), %rax
	jmp	.Lrow_sum_first.1
.Lrow_sum_first.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_sum_first, .-row_sum_first
	.globl	row_sum_register
	.type	row_sum_register, @function
row_sum_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	-16(%rbp), %rax
	movslq	-20(%rbp), %rcx
	leaq	12(%rax,%rcx,4), %rbx
	movq	%rbx, %rax
	jmp	.Lrow_sum_register.1
.Lrow_sum_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_sum_register, .-row_sum_register
	.globl	row_sum_stored
	.type	row_sum_stored, @function
row_sum_stored:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	-8(%rbp), %rcx
	movslq	-12(%rbp), %rdx
	leaq	12(%rcx,%rdx,4), %rcx
	movq	%rcx, (%rax)
.Lrow_sum_stored.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_sum_stored, .-row_sum_stored
	.globl	row_sum_wide
	.type	row_sum_wide, @function
row_sum_wide:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rax
	leaq	24(%rax), %rax
	movslq	-12(%rbp), %rcx
	imulq	$12, %rcx, %rcx
	addq	%rcx, %rax
	jmp	.Lrow_sum_wide.1
.Lrow_sum_wide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_sum_wide, .-row_sum_wide
	.section	.note.GNU-stack,"",@progbits
