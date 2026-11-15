	.file	"call-pointer-stack.c"
	.text
	.globl	nine
	.type	nine, @function
nine:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	mov	w0, #1
	str	w0, [x29, #32]
	mov	w0, #2
	str	w0, [x29, #36]
	mov	w0, #3
	str	w0, [x29, #40]
	mov	w0, #4
	str	w0, [x29, #44]
	mov	w0, #5
	str	w0, [x29, #48]
	mov	w0, #6
	str	w0, [x29, #52]
	mov	w0, #7
	str	w0, [x29, #56]
	mov	w0, #8
	str	w0, [x29, #60]
	mov	w0, #9
	str	w0, [x29, #64]
	sub	sp, sp, #16
	ldr	w17, [x29, #64]
	str	w17, [sp]
	ldr	w0, [x29, #32]
	ldr	w1, [x29, #36]
	ldr	w2, [x29, #40]
	ldr	w3, [x29, #44]
	ldr	w4, [x29, #48]
	ldr	w5, [x29, #52]
	ldr	w6, [x29, #56]
	ldr	w7, [x29, #60]
	ldr	x16, [x29, #24]
	blr	x16
	add	sp, sp, #16
	b	.Lnine.1
.Lnine.1:
	.cfi_def_cfa wsp, 80
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nine, .-nine
	.section	.note.GNU-stack,"",@progbits
