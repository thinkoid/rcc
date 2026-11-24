	.file	"bit-field-statement-expression-value.c"
	.text
	.globl	store
	.type	store, @function
store:
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
	ldr	w1, [x29, #24]
	lsl	w1, w1, #3
	ldr	x2, [x29, #16]
	ldrsb	w2, [x2]
	and	w2, w2, #0xffffff07
	orr	w1, w1, w2
	strb	w1, [x0]
	sxtb	w1, w1
	asr	w0, w1, #3
	b	.Lstore.1
.Lstore.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	store, .-store
	.section	.note.GNU-stack,"",@progbits
