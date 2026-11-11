	.file	"stack-args.c"
	.text
	.globl	seven
	.type	seven, @function
seven:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
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
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	bl	f7
	b	.Lseven.1
.Lseven.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	seven, .-seven
	.globl	eight
	.type	eight, @function
eight:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
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
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	ldr	w7, [x29, #44]
	bl	f8
	b	.Leight.1
.Leight.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	eight, .-eight
	.section	.note.GNU-stack,"",@progbits
