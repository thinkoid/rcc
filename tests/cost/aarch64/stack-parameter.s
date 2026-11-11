	.file	"stack-parameter.c"
	.text
	.globl	ten
	.type	ten, @function
ten:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	ldrb	w17, [x29, #64]
	strb	w17, [x29, #48]
	ldr	x17, [x29, #72]
	str	x17, [x29, #56]
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	str	w4, [x29, #32]
	str	w5, [x29, #36]
	str	w6, [x29, #40]
	str	w7, [x29, #44]
	ldr	w0, [x29, #16]
	ldrb	w1, [x29, #48]
	add	w0, w0, w1
	sxtw	x0, w0
	ldr	x1, [x29, #56]
	add	x0, x0, x1
	b	.Lten.1
.Lten.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	ten, .-ten
	.section	.note.GNU-stack,"",@progbits
