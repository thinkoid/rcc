	.file	"parameter-array-star.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	w0, [x0]
	b	.Lf.1
.Lf.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	f, .-f
	.section	.note.GNU-stack,"",@progbits
