	.file	"unused.c"
	.text
	.globl	unused
	.type	unused, @function
unused:
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
	add	w0, w0, #0
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	sdiv	w0, w0, w1
	mov	w0, #0
	b	.Lunused.1
.Lunused.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unused, .-unused
	.section	.note.GNU-stack,"",@progbits
