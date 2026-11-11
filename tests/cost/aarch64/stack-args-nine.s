	.file	"stack-args-nine.c"
	.text
	.globl	nine
	.type	nine, @function
nine:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #24]
	mov	w0, #4
	str	w0, [x29, #28]
	mov	w0, #5
	str	w0, [x29, #32]
	mov	w0, #6
	str	w0, [x29, #36]
	mov	w0, #7
	str	w0, [x29, #40]
	mov	w0, #8
	str	w0, [x29, #44]
	mov	w0, #9
	strb	w0, [x29, #48]
	sub	sp, sp, #16
	ldrb	w17, [x29, #48]
	strb	w17, [sp]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	ldr	w7, [x29, #44]
	bl	f9
	add	sp, sp, #16
	b	.Lnine.1
.Lnine.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nine, .-nine
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
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #24]
	mov	w0, #4
	str	w0, [x29, #28]
	mov	w0, #5
	str	w0, [x29, #32]
	mov	w0, #6
	str	w0, [x29, #36]
	mov	w0, #7
	str	w0, [x29, #40]
	mov	w0, #8
	str	w0, [x29, #44]
	mov	w0, #9
	str	w0, [x29, #48]
	mov	w0, #10
	str	w0, [x29, #52]
	sub	sp, sp, #16
	ldr	w17, [x29, #48]
	str	w17, [sp]
	ldr	w17, [x29, #52]
	str	w17, [sp, #8]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	ldr	w7, [x29, #44]
	bl	f10
	add	sp, sp, #16
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
