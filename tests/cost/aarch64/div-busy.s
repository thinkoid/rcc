	.file	"div-busy.c"
	.text
	.globl	div_busy
	.type	div_busy, @function
div_busy:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	b	.Ldiv_busy.1
.Ldiv_busy.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div_busy, .-div_busy
	.globl	rem_busy
	.type	rem_busy, @function
rem_busy:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w3, w1, w2
	msub	w1, w3, w2, w1
	add	w0, w0, w1
	b	.Lrem_busy.1
.Lrem_busy.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_busy, .-rem_busy
	.globl	div_deeper
	.type	div_deeper, @function
div_deeper:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	add	w1, w1, #1
	add	w0, w0, w1
	b	.Ldiv_deeper.1
.Ldiv_deeper.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div_deeper, .-div_deeper
	.globl	div_held
	.type	div_held, @function
div_held:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	str	w4, [x29, #32]
	str	w5, [x29, #36]
	str	w6, [x29, #40]
	str	w7, [x29, #44]
	ldr	w17, [x29, #64]
	str	w17, [x29, #48]
	ldr	w17, [x29, #72]
	str	w17, [x29, #52]
	ldr	w17, [x29, #80]
	str	w17, [x29, #56]
	ldr	w17, [x29, #88]
	str	w17, [x29, #60]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	ldr	w7, [x29, #44]
	ldr	w9, [x29, #48]
	ldr	w10, [x29, #52]
	sdiv	w9, w9, w10
	add	w7, w7, w9
	add	w6, w6, w7
	add	w5, w5, w6
	add	w4, w4, w5
	add	w3, w3, w4
	add	w2, w2, w3
	add	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #56]
	ldr	w2, [x29, #60]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	b	.Ldiv_held.1
.Ldiv_held.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div_held, .-div_held
	.globl	rem_stored
	.type	rem_stored, @function
rem_stored:
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
	str	w2, [x29, #28]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldr	w2, [x29, #28]
	sdiv	w3, w1, w2
	msub	w1, w3, w2, w1
	str	w1, [x0]
.Lrem_stored.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_stored, .-rem_stored
	.globl	rem_returned
	.type	rem_returned, @function
rem_returned:
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
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lrem_returned.1
.Lrem_returned.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_returned, .-rem_returned
	.globl	rem_left
	.type	rem_left, @function
rem_left:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	ldr	w1, [x29, #16]
	add	w0, w0, w1
	b	.Lrem_left.1
.Lrem_left.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_left, .-rem_left
	.globl	rem_argument
	.type	rem_argument, @function
rem_argument:
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
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	bl	f
	b	.Lrem_argument.1
.Lrem_argument.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_argument, .-rem_argument
	.globl	rem_quotient
	.type	rem_quotient, @function
rem_quotient:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	ldr	w1, [x29, #24]
	sdiv	w0, w0, w1
	b	.Lrem_quotient.1
.Lrem_quotient.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_quotient, .-rem_quotient
	.globl	rem_remainder
	.type	rem_remainder, @function
rem_remainder:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lrem_remainder.1
.Lrem_remainder.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_remainder, .-rem_remainder
	.globl	rem_register
	.type	rem_register, @function
rem_register:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	sdiv	w1, w19, w0
	msub	w19, w1, w0, w19
	mov	w0, w19
	b	.Lrem_register.1
.Lrem_register.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_register, .-rem_register
	.globl	rem_local
	.type	rem_local, @function
rem_local:
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
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w1, w0
	msub	w1, w2, w0, w1
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	b	.Lrem_local.1
.Lrem_local.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_local, .-rem_local
	.globl	rem_arm
	.type	rem_arm, @function
rem_arm:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	cbz	w0, .Lrem_arm.2
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lrem_arm.3
.Lrem_arm.2:
	ldr	w0, [x29, #24]
.Lrem_arm.3:
	b	.Lrem_arm.1
.Lrem_arm.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_arm, .-rem_arm
	.globl	quotient_nested
	.type	quotient_nested, @function
quotient_nested:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	sdiv	w0, w0, w1
	b	.Lquotient_nested.1
.Lquotient_nested.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	quotient_nested, .-quotient_nested
	.globl	quotient_compound
	.type	quotient_compound, @function
quotient_compound:
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
	bl	f
	ldr	w1, [x29, #16]
	sdiv	w0, w1, w0
	str	w0, [x29, #16]
	b	.Lquotient_compound.1
.Lquotient_compound.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	quotient_compound, .-quotient_compound
	.globl	remainder_called
	.type	remainder_called, @function
remainder_called:
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
	bl	f
	ldr	x1, [x29, #24]
	sdiv	w2, w1, w0
	msub	w0, w2, w0, w1
	b	.Lremainder_called.1
.Lremainder_called.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	remainder_called, .-remainder_called
	.section	.note.GNU-stack,"",@progbits
