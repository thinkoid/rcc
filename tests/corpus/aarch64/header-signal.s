	.file	"header-signal.c"
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
	str	wzr, [x29, #16]
	ldr	w0, [x29, #16]
	bl	raise
	cmp	w0, #0
	b.eq	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	mov	w0, #10
	str	w0, [x29, #20]
	mov	x0, #1
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	x1, [x29, #24]
	bl	signal
	cmp	x0, #0
	b.eq	.Lmain.3
	mov	w0, #2
	b	.Lmain.1
.Lmain.3:
	mov	w0, #10
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	raise
	cmp	w0, #0
	b.eq	.Lmain.4
	mov	w0, #3
	b	.Lmain.1
.Lmain.4:
	mov	w0, #10
	str	w0, [x29, #36]
	str	xzr, [x29, #40]
	ldr	w0, [x29, #36]
	ldr	x1, [x29, #40]
	bl	signal
	cmp	x0, #1
	b.eq	.Lmain.5
	mov	w0, #4
	b	.Lmain.1
.Lmain.5:
	mov	w0, #0
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
