	.file	"bit-field-compound-call.c"
	.text
	.globl	raise
	.type	raise, @function
raise:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	bl	current
	add	x0, x0, #4
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldrb	w0, [x0]
	strb	w0, [x29, #20]
	ldr	x0, [x29, #24]
	ldrb	w1, [x29, #20]
	lsl	w1, w1, #28
	lsr	w1, w1, #29
	ldr	w2, [x29, #16]
	add	w1, w1, w2
	lsl	w1, w1, #1
	and	w1, w1, #0xe
	ldrb	w2, [x29, #20]
	and	w2, w2, #0xfffffff1
	orr	w1, w1, w2
	strb	w1, [x0]
.Lraise.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	raise, .-raise
	.section	.note.GNU-stack,"",@progbits
