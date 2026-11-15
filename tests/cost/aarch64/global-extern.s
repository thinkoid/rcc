	.file	"global-extern.c"
	.text
	.globl	get
	.type	get, @function
get:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, :got:counter
	ldr	x0, [x0, :got_lo12:counter]
	ldr	w0, [x0]
	b	.Lget.1
.Lget.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	get, .-get
	.globl	set
	.type	set, @function
set:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, :got:counter
	ldr	x0, [x0, :got_lo12:counter]
	ldr	w1, [x29, #16]
	str	w1, [x0]
.Lset.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	set, .-set
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, :got:counter
	ldr	x0, [x0, :got_lo12:counter]
	ldr	w1, [x29, #16]
	ldr	w2, [x0]
	add	w2, w2, w1
	str	w2, [x0]
.Ladd.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add, .-add
	.globl	step
	.type	step, @function
step:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, :got:counter
	ldr	x0, [x0, :got_lo12:counter]
	ldr	w1, [x0]
	add	w1, w1, #1
	str	w1, [x0]
.Lstep.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step, .-step
	.globl	address
	.type	address, @function
address:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, :got:counter
	ldr	x0, [x0, :got_lo12:counter]
	b	.Laddress.1
.Laddress.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	address, .-address
	.globl	increment
	.type	increment, @function
increment:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, :got:counter
	ldr	x0, [x0, :got_lo12:counter]
	adrp	x1, :got:counter
	ldr	x1, [x1, :got_lo12:counter]
	ldr	w1, [x1]
	add	w1, w1, #1
	str	w1, [x0]
.Lincrement.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	increment, .-increment
	.section	.note.GNU-stack,"",@progbits
