	.file	"subscript-row.c"
	.text
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	b	.Lfirst.1
.Lfirst.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	first, .-first
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	ldr	w0, [x29, #20]
	b	.Lelement.1
.Lelement.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	second
	.type	second, @function
second:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #28
	b	.Lsecond.1
.Lsecond.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	second, .-second
	.globl	through
	.type	through, @function
through:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	add	x0, x0, #12
	b	.Lthrough.1
.Lthrough.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	through, .-through
	.globl	beyond
	.type	beyond, @function
beyond:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	mov	x1, #6144
	movk	x1, #0x8f0d, lsl #16
	add	x0, x0, x1
	b	.Lbeyond.1
.Lbeyond.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	beyond, .-beyond
	.globl	column
	.type	column, @function
column:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #12
	ldr	w1, [x29, #24]
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lcolumn.1
.Lcolumn.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	column, .-column
	.globl	column_first
	.type	column_first, @function
column_first:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	ldr	x1, [x29, #16]
	add	x1, x1, #12
	ldr	w0, [x1, w0, sxtw #2]
	b	.Lcolumn_first.1
.Lcolumn_first.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	column_first, .-column_first
	.globl	column_beyond
	.type	column_beyond, @function
column_beyond:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	mov	x1, #6144
	movk	x1, #0x8f0d, lsl #16
	add	x0, x0, x1
	ldr	w1, [x29, #24]
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lcolumn_beyond.1
.Lcolumn_beyond.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	column_beyond, .-column_beyond
	.globl	column_sum
	.type	column_sum, @function
column_sum:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #12
	mov	x1, #0x7ffffffc
	add	x0, x0, x1
	ldr	w1, [x29, #24]
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lcolumn_sum.1
.Lcolumn_sum.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	column_sum, .-column_sum
	.globl	column_wide
	.type	column_wide, @function
column_wide:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #24
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	b	.Lcolumn_wide.1
.Lcolumn_wide.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	column_wide, .-column_wide
	.globl	row_sum
	.type	row_sum, @function
row_sum:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #12
	ldr	w1, [x29, #24]
	add	x0, x0, w1, sxtw #2
	b	.Lrow_sum.1
.Lrow_sum.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_sum, .-row_sum
	.globl	row_sum_first
	.type	row_sum_first, @function
row_sum_first:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	ldr	x1, [x29, #16]
	add	x1, x1, #12
	add	x0, x1, w0, sxtw #2
	b	.Lrow_sum_first.1
.Lrow_sum_first.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_sum_first, .-row_sum_first
	.globl	row_sum_register
	.type	row_sum_register, @function
row_sum_register:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	w1, [x29, #32]
	ldr	x0, [x29, #24]
	add	x0, x0, #12
	ldr	w1, [x29, #32]
	add	x19, x0, w1, sxtw #2
	mov	x0, x19
	b	.Lrow_sum_register.1
.Lrow_sum_register.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_sum_register, .-row_sum_register
	.globl	row_sum_stored
	.type	row_sum_stored, @function
row_sum_stored:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	x2, [x29, #32]
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #16]
	add	x1, x1, #12
	ldr	w2, [x29, #24]
	add	x1, x1, w2, sxtw #2
	str	x1, [x0]
.Lrow_sum_stored.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_sum_stored, .-row_sum_stored
	.globl	row_sum_wide
	.type	row_sum_wide, @function
row_sum_wide:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #24
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	b	.Lrow_sum_wide.1
.Lrow_sum_wide.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_sum_wide, .-row_sum_wide
	.globl	cell
	.type	cell, @function
cell:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #36
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0]
	b	.Lcell.1
.Lcell.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	cell, .-cell
	.globl	cell_column
	.type	cell_column, @function
cell_column:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #36
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	b	.Lcell_column.1
.Lcell_column.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	cell_column, .-cell_column
	.globl	cell_register
	.type	cell_register, @function
cell_register:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	w1, [x29, #32]
	ldr	x19, [x29, #24]
	add	x0, x19, #36
	ldrsw	x1, [x29, #32]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0]
	b	.Lcell_register.1
.Lcell_register.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	cell_register, .-cell_register
	.globl	cell_first
	.type	cell_first, @function
cell_first:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldrsw	x0, [x29, #24]
	ldr	x1, [x29, #16]
	add	x1, x1, #36
	mov	x2, #12
	mul	x0, x0, x2
	add	x0, x0, x1
	ldr	w0, [x0]
	b	.Lcell_first.1
.Lcell_first.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	cell_first, .-cell_first
	.section	.note.GNU-stack,"",@progbits
