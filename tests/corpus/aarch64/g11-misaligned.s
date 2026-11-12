	.file	"g11-misaligned.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #28]
.Lmain.2:
	ldr	w0, [x29, #28]
	cmp	w0, #8
	b.ge	.Lmain.4
	add	x0, x29, #20
	ldr	w1, [x29, #28]
	ldr	w2, [x29, #28]
	ldr	w3, [x29, #16]
	add	w2, w2, w3
	strb	w2, [x0, w1, sxtw]
.Lmain.3:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lmain.2
.Lmain.4:
	add	x0, x29, #20
	add	x0, x0, #1
	ldr	w0, [x0]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	asr	w0, w0, #24
	mov	w1, #10
	mul	w0, w0, w1
	ldr	w1, [x29, #32]
	and	w1, w1, #0xff
	add	w0, w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
