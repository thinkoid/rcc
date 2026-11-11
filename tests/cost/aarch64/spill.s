	.file	"spill.c"
	.text
	.globl	spill
	.type	spill, @function
spill:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	str	w4, [x29, #32]
	str	w5, [x29, #36]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #16]
	ldr	w7, [x29, #20]
	ldr	w9, [x29, #24]
	ldr	w10, [x29, #28]
	ldr	w11, [x29, #32]
	sub	w10, w10, w11
	sub	w9, w9, w10
	sub	w7, w7, w9
	sub	w6, w6, w7
	sub	w5, w5, w6
	sub	w4, w4, w5
	sub	w3, w3, w4
	sub	w2, w2, w3
	sub	w1, w1, w2
	sub	w0, w0, w1
	b	.Lspill.1
.Lspill.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spill, .-spill
	.section	.note.GNU-stack,"",@progbits
