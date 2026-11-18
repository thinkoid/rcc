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
	movslq	-12(%rbp), %rcx
	imulq	$12, %rcx, %rcx
	leaq	24(%rax,%rcx,1), %rax
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
	movslq	-12(%rbp), %rcx
	imulq	$12, %rcx, %rcx
	leaq	24(%rax,%rcx,1), %rax
	jmp	.Lrow_sum_wide.1
.Lrow_sum_wide.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	row_sum_wide, .-row_sum_wide
	.globl	cell
	.type	cell, @function
cell:
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
	imulq	$12, %rcx, %rcx
	movl	36(%rax,%rcx,1), %eax
	jmp	.Lcell.1
.Lcell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	cell, .-cell
	.globl	cell_column
	.type	cell_column, @function
cell_column:
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
	imulq	$12, %rcx, %rcx
	movl	44(%rax,%rcx,1), %eax
	jmp	.Lcell_column.1
.Lcell_column.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	cell_column, .-cell_column
	.globl	cell_register
	.type	cell_register, @function
cell_register:
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
	movq	-16(%rbp), %rbx
	movslq	-20(%rbp), %rax
	imulq	$12, %rax, %rax
	movl	36(%rbx,%rax,1), %eax
	jmp	.Lcell_register.1
.Lcell_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	cell_register, .-cell_register
	.globl	cell_first
	.type	cell_first, @function
cell_first:
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
	leaq	36(%rcx), %rcx
	imulq	$12, %rax, %rax
	addq	%rcx, %rax
	movl	(%rax), %eax
	jmp	.Lcell_first.1
.Lcell_first.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	cell_first, .-cell_first
	.globl	slot_cell
	.type	slot_cell, @function
slot_cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	imulq	$5, %rax, %rax
	movsbl	-20(%rbp,%rax,1), %eax
	jmp	.Lslot_cell.1
.Lslot_cell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	slot_cell, .-slot_cell
	.globl	label_cell
	.type	label_cell, @function
label_cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	leaq	rows(%rip), %rax
	movslq	-4(%rbp), %rcx
	imulq	$5, %rcx, %rcx
	movsbl	2(%rax,%rcx,1), %eax
	jmp	.Llabel_cell.1
.Llabel_cell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	label_cell, .-label_cell
	.globl	pointer_cell
	.type	pointer_cell, @function
pointer_cell:
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
	imulq	$12, %rcx, %rcx
	movl	4(%rax,%rcx,1), %eax
	jmp	.Lpointer_cell.1
.Lpointer_cell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	pointer_cell, .-pointer_cell
	.globl	long_cell
	.type	long_cell, @function
long_cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	imulq	$12, -16(%rbp), %rcx
	movl	8(%rax,%rcx,1), %eax
	jmp	.Llong_cell.1
.Llong_cell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	long_cell, .-long_cell
	.globl	slot_row
	.type	slot_row, @function
slot_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	imulq	$12, %rax, %rax
	leaq	-52(%rbp,%rax,1), %rax
	jmp	.Lslot_row.1
.Lslot_row.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	slot_row, .-slot_row
	.globl	first_cell
	.type	first_cell, @function
first_cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movslq	-12(%rbp), %rax
	imulq	$12, %rax, %rax
	addq	-8(%rbp), %rax
	movl	4(%rax), %eax
	jmp	.Lfirst_cell.1
.Lfirst_cell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	first_cell, .-first_cell
	.globl	held_cell
	.type	held_cell, @function
held_cell:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	leaq	-24(%rbp), %rax
	movslq	-4(%rbp), %rcx
	imulq	$5, %rcx, %rcx
	addq	%rcx, %rax
	movq	%rax, -32(%rbp)
	call	f@PLT
	movq	-32(%rbp), %rcx
	subb	%al, 1(%rcx)
.Lheld_cell.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	held_cell, .-held_cell
	.globl	register_row
	.type	register_row, @function
register_row:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	%edi, -12(%rbp)
	movslq	-12(%rbp), %rax
	imulq	$12, %rax, %rax
	leaq	-60(%rbp,%rax,1), %rbx
	movq	%rbx, %rax
	jmp	.Lregister_row.1
.Lregister_row.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	register_row, .-register_row
	.globl	byte_sum
	.type	byte_sum, @function
byte_sum:
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
	leaq	8(%rax,%rcx,1), %rax
	jmp	.Lbyte_sum.1
.Lbyte_sum.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_sum, .-byte_sum
	.globl	byte_sum_long
	.type	byte_sum_long, @function
byte_sum_long:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	leaq	8(%rax), %rax
	addq	-16(%rbp), %rax
	jmp	.Lbyte_sum_long.1
.Lbyte_sum_long.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_sum_long, .-byte_sum_long
	.globl	byte_sum_register
	.type	byte_sum_register, @function
byte_sum_register:
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
	leaq	8(%rax,%rcx,1), %rbx
	movq	%rbx, %rax
	jmp	.Lbyte_sum_register.1
.Lbyte_sum_register.1:
	popq	%rbx
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	byte_sum_register, .-byte_sum_register
	.bss
	.globl	rows
	.type	rows, @object
	.size	rows, 20
	.p2align 4
rows:
	.zero	20
	.section	.note.GNU-stack,"",@progbits
