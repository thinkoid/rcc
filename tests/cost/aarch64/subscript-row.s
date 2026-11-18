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
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #36]
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
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #44]
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
	ldrsw	x0, [x29, #32]
	mov	x1, #12
	mul	x0, x0, x1
	add	x0, x19, x0
	ldr	w0, [x0, #36]
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
	mov	x2, #12
	mul	x0, x0, x2
	add	x0, x0, x1
	ldr	w0, [x0, #36]
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
	.globl	slot_cell
	.type	slot_cell, @function
slot_cell:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldrsw	x0, [x29, #16]
	mov	x1, #5
	mul	x0, x0, x1
	add	x0, x29, x0
	ldrb	w0, [x0, #24]
	b	.Lslot_cell.1
.Lslot_cell.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	slot_cell, .-slot_cell
	.globl	label_cell
	.type	label_cell, @function
label_cell:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, rows+2
	ldrsw	x1, [x29, #16]
	mov	x2, #5
	mul	x1, x1, x2
	add	x0, x0, x1
	ldrb	w0, [x0, :lo12:rows+2]
	b	.Llabel_cell.1
.Llabel_cell.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	label_cell, .-label_cell
	.globl	pointer_cell
	.type	pointer_cell, @function
pointer_cell:
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
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #4]
	b	.Lpointer_cell.1
.Lpointer_cell.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_cell, .-pointer_cell
	.globl	long_cell
	.type	long_cell, @function
long_cell:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	b	.Llong_cell.1
.Llong_cell.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	long_cell, .-long_cell
	.globl	slot_row
	.type	slot_row, @function
slot_row:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	add	x0, x29, #20
	ldrsw	x1, [x29, #16]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	b	.Lslot_row.1
.Lslot_row.1:
	.cfi_def_cfa wsp, 80
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	slot_row, .-slot_row
	.globl	first_cell
	.type	first_cell, @function
first_cell:
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
	mov	x2, #12
	mul	x0, x0, x2
	add	x0, x0, x1
	ldr	w0, [x0, #4]
	b	.Lfirst_cell.1
.Lfirst_cell.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	first_cell, .-first_cell
	.globl	held_cell
	.type	held_cell, @function
held_cell:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldrsw	x0, [x29, #16]
	mov	x1, #5
	mul	x0, x0, x1
	add	x0, x29, x0
	str	x0, [x29, #40]
	bl	f
	ldr	x2, [x29, #40]
	ldrb	w1, [x2, #21]
	sub	w1, w1, w0
	strb	w1, [x2, #21]
.Lheld_cell.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held_cell, .-held_cell
	.globl	register_row
	.type	register_row, @function
register_row:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -64
	str	w0, [x29, #24]
	add	x0, x29, #28
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x19, x0, x1
	mov	x0, x19
	b	.Lregister_row.1
.Lregister_row.1:
	.cfi_def_cfa wsp, 80
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	register_row, .-register_row
	.globl	byte_sum
	.type	byte_sum, @function
byte_sum:
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
	add	x0, x0, #8
	ldr	w1, [x29, #24]
	add	x0, x0, w1, sxtw
	b	.Lbyte_sum.1
.Lbyte_sum.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	byte_sum, .-byte_sum
	.globl	byte_sum_long
	.type	byte_sum_long, @function
byte_sum_long:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #8
	ldr	x1, [x29, #24]
	add	x0, x0, x1
	b	.Lbyte_sum_long.1
.Lbyte_sum_long.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	byte_sum_long, .-byte_sum_long
	.globl	byte_sum_register
	.type	byte_sum_register, @function
byte_sum_register:
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
	add	x0, x0, #8
	ldr	w1, [x29, #32]
	add	x19, x0, w1, sxtw
	mov	x0, x19
	b	.Lbyte_sum_register.1
.Lbyte_sum_register.1:
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
	.size	byte_sum_register, .-byte_sum_register
	.globl	row_start
	.type	row_start, @function
row_start:
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
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	ldr	w0, [x0, x1]
	b	.Lrow_start.1
.Lrow_start.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_start, .-row_start
	.globl	row_start_stored
	.type	row_start_stored, @function
row_start_stored:
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
	ldr	x0, [x29, #16]
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	str	x0, [x29, #32]
	bl	f
	ldr	x1, [x29, #32]
	str	w0, [x1]
.Lrow_start_stored.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_start_stored, .-row_start_stored
	.bss
	.globl	rows
	.type	rows, @object
	.size	rows, 20
rows:
	.zero	20
	.section	.note.GNU-stack,"",@progbits
