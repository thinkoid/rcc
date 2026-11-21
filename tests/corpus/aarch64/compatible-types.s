	.file	"compatible-types.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	mov	w0, #12
	b	.Lf.1
.Lf.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	f, .-f
	.globl	pick
	.type	pick, @function
pick:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	mov	w0, #12
	b	.Lpick.1
.Lpick.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	through_long_long
	.type	through_long_long, @function
through_long_long:
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
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	x0, [x0]
	b	.Lthrough_long_long.1
.Lthrough_long_long.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	through_long_long, .-through_long_long
	.globl	through_shape
	.type	through_shape, @function
through_shape:
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
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	w0, [x0]
	b	.Lthrough_shape.1
.Lthrough_shape.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	through_shape, .-through_shape
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	x0, #5
	str	x0, [x29, #16]
	mov	w0, #1
	str	w0, [x29, #24]
	mov	w0, #1
	adrp	x16, count
	str	w0, [x16, :lo12:count]
	mov	w0, #1
	adrp	x16, total
	str	w0, [x16, :lo12:total]
	adrp	x0, grid
	add	x0, x0, :lo12:grid
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	f
	cmp	w0, #12
	b.eq	.Lmain.2
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	str	wzr, [x29, #40]
	ldr	w0, [x29, #40]
	bl	pick
	cmp	w0, #12
	b.eq	.Lmain.3
	mov	w0, #102
	b	.Lmain.1
.Lmain.3:
	add	x0, x29, #16
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	through_long_long
	cmp	x0, #5
	b.eq	.Lmain.4
	mov	w0, #103
	b	.Lmain.1
.Lmain.4:
	add	x0, x29, #24
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	bl	through_shape
	cmp	w0, #1
	b.eq	.Lmain.5
	mov	w0, #104
	b	.Lmain.1
.Lmain.5:
	adrp	x16, count
	ldr	w0, [x16, :lo12:count]
	cmp	w0, #1
	b.eq	.Lmain.6
	mov	w0, #105
	b	.Lmain.1
.Lmain.6:
	adrp	x16, total
	ldr	w0, [x16, :lo12:total]
	cmp	w0, #1
	b.eq	.Lmain.7
	mov	w0, #106
	b	.Lmain.1
.Lmain.7:
	mov	w0, #42
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
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
