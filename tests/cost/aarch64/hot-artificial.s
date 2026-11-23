	.file	"hot-artificial.c"
	.text
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	bl	warm
	str	x0, [x29, #24]
	ldr	w0, [x29, #16]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	plain
	ldr	x1, [x29, #24]
	add	w0, w1, w0
	b	.Lboth.1
.Lboth.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	both, .-both
	.section	.note.GNU-stack,"",@progbits
