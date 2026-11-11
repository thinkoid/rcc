	.file	"compound.c"
	.text
	.globl	compound
	.type	compound, @function
compound:
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
	add	w1, w1, w0
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	b	.Lcompound.1
.Lcompound.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compound, .-compound
	.globl	narrow
	.type	narrow, @function
narrow:
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
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	add	w1, w1, w0
	strb	w1, [x29, #24]
	ldrb	w0, [x29, #24]
	b	.Lnarrow.1
.Lnarrow.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	in_value
	.type	in_value, @function
in_value:
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
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	add	w1, w1, w0
	strb	w1, [x29, #24]
	uxtb	w1, w1
	mov	w0, w1
	b	.Lin_value.1
.Lin_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_value, .-in_value
	.section	.note.GNU-stack,"",@progbits
