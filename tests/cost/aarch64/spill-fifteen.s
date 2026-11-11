	.file	"spill-fifteen.c"
	.text
	.globl	spill_fifteen
	.type	spill_fifteen, @function
spill_fifteen:
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
	str	w3, [x29, #28]
	str	w4, [x29, #32]
	str	w5, [x29, #36]
	str	w6, [x29, #40]
	str	w7, [x29, #44]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	ldr	w7, [x29, #44]
	ldr	w9, [x29, #16]
	ldr	w10, [x29, #20]
	ldr	w11, [x29, #24]
	ldr	w12, [x29, #28]
	ldr	w13, [x29, #32]
	ldr	w14, [x29, #36]
	ldr	w15, [x29, #40]
	str	x0, [x29, #48]
	ldr	w0, [x29, #44]
	str	x1, [x29, #56]
	ldr	w1, [x29, #16]
	sub	w0, w0, w1
	sub	w15, w15, w0
	sub	w14, w14, w15
	sub	w13, w13, w14
	sub	w12, w12, w13
	sub	w11, w11, w12
	sub	w10, w10, w11
	sub	w9, w9, w10
	sub	w7, w7, w9
	sub	w6, w6, w7
	sub	w5, w5, w6
	sub	w4, w4, w5
	sub	w3, w3, w4
	sub	w2, w2, w3
	ldr	x0, [x29, #56]
	sub	w0, w0, w2
	ldr	x1, [x29, #48]
	sub	w1, w1, w0
	mov	w0, w1
	b	.Lspill_fifteen.1
.Lspill_fifteen.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spill_fifteen, .-spill_fifteen
	.section	.note.GNU-stack,"",@progbits
