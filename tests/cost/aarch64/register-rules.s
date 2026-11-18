	.file	"register-rules.c"
	.text
	.globl	compound_in_place
	.type	compound_in_place, @function
compound_in_place:
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
	ldr	w19, [x29, #24]
	add	w19, w19, #5
	add	w19, w19, #1
	mov	w0, w19
	b	.Lcompound_in_place.1
.Lcompound_in_place.1:
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
	.size	compound_in_place, .-compound_in_place
	.globl	narrow_store
	.type	narrow_store, @function
narrow_store:
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
	uxtb	w19, w19
	ldr	w0, [x29, #24]
	str	w0, [x29, #28]
	ldr	w0, [x29, #28]
	bl	g
	uxtb	w19, w0
	mov	w0, w19
	b	.Lnarrow_store.1
.Lnarrow_store.1:
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
	.size	narrow_store, .-narrow_store
	.globl	narrow_step
	.type	narrow_step, @function
narrow_step:
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
	strb	w0, [x29, #24]
	ldrb	w19, [x29, #24]
	add	w19, w19, #1
	uxtb	w19, w19
	mov	w0, w19
	b	.Lnarrow_step.1
.Lnarrow_step.1:
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
	.size	narrow_step, .-narrow_step
	.globl	narrow_unwritten
	.type	narrow_unwritten, @function
narrow_unwritten:
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
	uxtb	w19, w19
	mov	w0, w19
	b	.Lnarrow_unwritten.1
.Lnarrow_unwritten.1:
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
	.size	narrow_unwritten, .-narrow_unwritten
	.globl	held_step
	.type	held_step, @function
held_step:
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
	ldr	w19, [x29, #24]
	mov	w0, w19
	mov	w1, w19
	add	w19, w19, #1
	add	w0, w0, w1
	b	.Lheld_step.1
.Lheld_step.1:
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
	.size	held_step, .-held_step
	.globl	held_and
	.type	held_and, @function
held_and:
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
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	str	x19, [x29, #32]
	ldr	w0, [x29, #28]
	cbz	w0, .Lheld_and.2
	mov	w0, w19
	add	w19, w19, #1
	cbz	w0, .Lheld_and.2
	mov	w0, #1
	b	.Lheld_and.3
.Lheld_and.2:
	mov	w0, #0
.Lheld_and.3:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	b	.Lheld_and.1
.Lheld_and.1:
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
	.size	held_and, .-held_and
	.globl	held_or
	.type	held_or, @function
held_or:
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
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	str	x19, [x29, #32]
	ldr	w0, [x29, #28]
	cbnz	w0, .Lheld_or.4
	mov	w0, w19
	add	w19, w19, #1
	cbz	w0, .Lheld_or.2
.Lheld_or.4:
	mov	w0, #1
	b	.Lheld_or.3
.Lheld_or.2:
	mov	w0, #0
.Lheld_or.3:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	b	.Lheld_or.1
.Lheld_or.1:
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
	.size	held_or, .-held_or
	.globl	held_conditional
	.type	held_conditional, @function
held_conditional:
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
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	str	x19, [x29, #32]
	ldr	w0, [x29, #28]
	cbz	w0, .Lheld_conditional.2
	mov	w0, w19
	add	w19, w19, #1
	b	.Lheld_conditional.3
.Lheld_conditional.2:
	mov	w0, #0
.Lheld_conditional.3:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	b	.Lheld_conditional.1
.Lheld_conditional.1:
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
	.size	held_conditional, .-held_conditional
	.globl	stacked
	.type	stacked, @function
stacked:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -48
	.cfi_offset w20, -40
	ldr	w20, [x29, #64]
	str	w0, [x29, #32]
	str	w1, [x29, #36]
	str	w2, [x29, #40]
	str	w3, [x29, #44]
	str	w4, [x29, #48]
	str	w5, [x29, #52]
	mov	w19, w6
	str	w7, [x29, #56]
	ldr	w0, [x29, #32]
	ldr	w1, [x29, #36]
	add	w0, w0, w1
	ldr	w1, [x29, #40]
	add	w0, w0, w1
	ldr	w1, [x29, #44]
	add	w0, w0, w1
	ldr	w1, [x29, #48]
	add	w0, w0, w1
	ldr	w1, [x29, #52]
	add	w0, w0, w1
	add	w0, w0, w19
	ldr	w1, [x29, #56]
	add	w0, w0, w1
	add	w0, w0, w20
	b	.Lstacked.1
.Lstacked.1:
	.cfi_def_cfa wsp, 64
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stacked, .-stacked
	.globl	mixed
	.type	mixed, @function
mixed:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	w0, [x29, #32]
	mov	w19, w1
	str	w2, [x29, #36]
	mov	w20, w3
	ldr	w0, [x29, #32]
	add	w0, w0, w19
	ldr	w1, [x29, #36]
	add	w0, w0, w1
	add	w0, w0, w20
	b	.Lmixed.1
.Lmixed.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mixed, .-mixed
	.globl	epilogue
	.type	epilogue, @function
epilogue:
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
	str	w0, [x29, #24]
	ldr	w19, [x29, #24]
	str	w19, [x29, #28]
	ldr	w0, [x29, #28]
	str	w0, [x29, #44]
	ldr	w0, [x29, #44]
	bl	g
	add	w0, w0, w19
	b	.Lepilogue.1
.Lepilogue.1:
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
	.size	epilogue, .-epilogue
	.globl	sixth
	.type	sixth, @function
sixth:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -64
	.cfi_offset w20, -56
	stp	x21, x22, [x29, #32]
	.cfi_offset w21, -48
	.cfi_offset w22, -40
	stp	x23, x24, [x29, #48]
	.cfi_offset w23, -32
	.cfi_offset w24, -24
	str	w0, [x29, #64]
	str	wzr, [x29, #68]
	ldr	w19, [x29, #64]
	ldr	w20, [x29, #64]
	add	w20, w20, #1
	ldr	w21, [x29, #64]
	add	w21, w21, #2
	ldr	w22, [x29, #64]
	add	w22, w22, #3
	ldr	w23, [x29, #64]
	add	w23, w23, #4
	ldr	w24, [x29, #64]
	add	w24, w24, #5
	add	w0, w19, w20
	add	w0, w0, w21
	add	w0, w0, w22
	add	w0, w0, w23
	add	w0, w0, w24
	str	w0, [x29, #68]
	ldr	w19, [x29, #64]
	mov	w0, #2
	mul	w19, w19, w0
	ldr	w0, [x29, #68]
	add	w0, w0, w19
	str	w0, [x29, #68]
	ldr	w0, [x29, #68]
	b	.Lsixth.1
.Lsixth.1:
	.cfi_def_cfa wsp, 80
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x21, x22, [x29, #32]
	.cfi_restore w21
	.cfi_restore w22
	ldp	x23, x24, [x29, #48]
	.cfi_restore w23
	.cfi_restore w24
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sixth, .-sixth
	.section	.note.GNU-stack,"",@progbits
