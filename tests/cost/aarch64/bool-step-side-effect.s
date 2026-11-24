	.file	"bool-step-side-effect.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
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
	add	x1, x0, #1
	str	x1, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #24]
	ldrb	w1, [x1]
	add	w1, w1, #1
	cmp	w1, #0
	cset	w1, ne
	strb	w1, [x0]
.Lmark.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mark, .-mark
	.section	.note.GNU-stack,"",@progbits
