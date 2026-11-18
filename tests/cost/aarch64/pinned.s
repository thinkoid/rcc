	.file	"pinned.c"
	.text
	.globl	fill
	.type	fill, @function
fill:
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
	adrp	x0, a
	add	x0, x0, :lo12:a
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	lsr	w2, w2, #16
	mov	w3, #256
	udiv	w4, w2, w3
	msub	w2, w4, w3, w2
	str	w2, [x0, w1, sxtw #2]
.Lfill.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fill, .-fill
	.globl	shifted
	.type	shifted, @function
shifted:
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
	add	x0, x29, #28
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	ldr	w3, [x29, #24]
	lsl	w2, w2, w3
	str	w2, [x0, w1, sxtw #2]
.Lshifted.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shifted, .-shifted
	.globl	shifted_through
	.type	shifted_through, @function
shifted_through:
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
	str	w2, [x29, #28]
	str	w3, [x29, #32]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldr	w2, [x29, #28]
	ldr	w3, [x29, #32]
	lsl	w2, w2, w3
	str	w2, [x0, w1, sxtw #2]
.Lshifted_through.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shifted_through, .-shifted_through
	.globl	divided_in_place
	.type	divided_in_place, @function
divided_in_place:
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
	ldr	w3, [x0, w1, sxtw #2]
	sdiv	w3, w3, w2
	str	w3, [x0, w1, sxtw #2]
.Ldivided_in_place.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	divided_in_place, .-divided_in_place
	.globl	shifted_in_place
	.type	shifted_in_place, @function
shifted_in_place:
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
	ldr	w3, [x0, w1, sxtw #2]
	lsl	w3, w3, w2
	str	w3, [x0, w1, sxtw #2]
.Lshifted_in_place.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shifted_in_place, .-shifted_in_place
	.globl	remainder_call
	.type	remainder_call, @function
remainder_call:
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
	str	x0, [x29, #24]
	bl	f
	ldr	w1, [x29, #20]
	ldr	w0, [x0, w1, sxtw #2]
	ldr	x1, [x29, #24]
	sdiv	w2, w1, w0
	msub	w0, w2, w0, w1
	b	.Lremainder_call.1
.Lremainder_call.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	remainder_call, .-remainder_call
	.globl	less
	.type	less, @function
less:
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
	cmp	w0, w1
	cset	w0, lt
	b	.Lless.1
.Lless.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	less, .-less
	.globl	subscript
	.type	subscript, @function
subscript:
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
	sdiv	w1, w1, w2
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lsubscript.1
.Lsubscript.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	subscript, .-subscript
	.bss
	.globl	a
	.type	a, @object
	.size	a, 32
	.p2align 2
a:
	.zero	32
	.section	.note.GNU-stack,"",@progbits
