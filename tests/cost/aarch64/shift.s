	.file	"shift.c"
	.text
	.globl	shl
	.type	shl, @function
shl:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	lsl	w0, w0, w1
	b	.Lshl.1
.Lshl.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shl, .-shl
	.globl	sar
	.type	sar, @function
sar:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	asr	w0, w0, w1
	b	.Lsar.1
.Lsar.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sar, .-sar
	.globl	shr
	.type	shr, @function
shr:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	lsr	w0, w0, w1
	b	.Lshr.1
.Lshr.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shr, .-shr
	.globl	count_extended
	.type	count_extended, @function
count_extended:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	narrow
	uxtb	w0, w0
	ldr	x1, [x29, #24]
	lsl	w0, w1, w0
	b	.Lcount_extended.1
.Lcount_extended.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count_extended, .-count_extended
	.globl	count_compound
	.type	count_compound, @function
count_compound:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #20]
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	bl	narrow
	uxtb	w0, w0
	ldr	w1, [x29, #16]
	lsl	w1, w1, w0
	str	w1, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Lcount_compound.1
.Lcount_compound.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count_compound, .-count_compound
	.globl	count_postfix
	.type	count_postfix, @function
count_postfix:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w2, w1, #1
	str	w2, [x29, #20]
	lsl	w0, w0, w1
	b	.Lcount_postfix.1
.Lcount_postfix.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count_postfix, .-count_postfix
	.globl	count_called
	.type	count_called, @function
count_called:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	wide
	ldr	x1, [x29, #24]
	lsl	w0, w1, w0
	b	.Lcount_called.1
.Lcount_called.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count_called, .-count_called
	.globl	count_held
	.type	count_held, @function
count_held:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	w3, [x29, #40]
	str	w4, [x29, #44]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	ldr	x2, [x29, #32]
	ldr	w3, [x29, #40]
	ldr	w4, [x29, #44]
	add	w4, w4, #1
	uxtb	w4, w4
	lsl	w3, w3, w4
	sxtw	x3, w3
	add	x2, x2, x3
	add	x1, x1, x2
	add	x0, x0, x1
	b	.Lcount_held.1
.Lcount_held.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count_held, .-count_held
	.section	.note.GNU-stack,"",@progbits
