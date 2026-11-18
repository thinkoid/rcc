	.file	"address-destination.c"
	.text
	.globl	row
	.type	row, @function
row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	12(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lrow.1
.Lrow.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row, .-row
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	4(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lelement.1
.Lelement.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	row_register
	.type	row_register, @function
row_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rbx
	leaq	12(%rbx), %r12
	movq	%r12, %rax
	jmp	.Lrow_register.1
.Lrow_register.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_register, .-row_register
	.globl	element_register
	.type	element_register, @function
element_register:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	pushq	%r12
	.cfi_offset %r12, -32
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rbx
	leaq	4(%rbx), %r12
	movq	%r12, %rax
	jmp	.Lelement_register.1
.Lelement_register.1:
	popq	%r12
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	element_register, .-element_register
	.globl	declared
	.type	declared, @function
declared:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	12(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Ldeclared.1
.Ldeclared.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	declared, .-declared
	.globl	through
	.type	through, @function
through:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	(%rax), %rbx
	leaq	8(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lthrough.1
.Lthrough.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	through, .-through
	.globl	itself
	.type	itself, @function
itself:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	4(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Litself.1
.Litself.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	itself, .-itself
	.globl	called
	.type	called, @function
called:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	subq	$24, %rsp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, -24(%rbp)
	movq	-24(%rbp), %rdi
	call	f@PLT
	leaq	4(%rax), %rbx
	movq	%rbx, %rax
	jmp	.Lcalled.1
.Lcalled.1:
	leaq	-8(%rbp), %rsp
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	global
	.type	global, @function
global:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	xa@GOTPCREL(%rip), %rbx
	leaq	12(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lglobal.1
.Lglobal.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	global, .-global
	.globl	global_sum
	.type	global_sum, @function
global_sum:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	xa@GOTPCREL(%rip), %rbx
	leaq	12(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lglobal_sum.1
.Lglobal_sum.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	global_sum, .-global_sum
	.globl	column
	.type	column, @function
column:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	leaq	20(%rbx), %rbx
	movq	%rbx, %rax
	jmp	.Lcolumn.1
.Lcolumn.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column, .-column
	.globl	column_zero
	.type	column_zero, @function
column_zero:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rbx
	movq	%rbx, %rax
	jmp	.Lcolumn_zero.1
.Lcolumn_zero.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	column_zero, .-column_zero
	.section	.note.GNU-stack,"",@progbits
