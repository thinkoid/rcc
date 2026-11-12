	.file	"g8-cross-object.c"
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
	mov	w0, #1
	str	w0, [x29, #20]
	mov	w0, #2
	str	w0, [x29, #24]
	add	x0, x29, #24
	add	x1, x29, #20
	sub	x0, x0, x1
	str	x0, [x29, #32]
	add	x0, x29, #20
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #32]
	add	x0, x0, x1
	mov	w1, #3
	str	w1, [x0]
	ldr	w0, [x29, #24]
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
