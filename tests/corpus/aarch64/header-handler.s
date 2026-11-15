	.file	"header-handler.c"
	.text
	.globl	handler
	.type	handler, @function
handler:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	cmp	w0, #10
	b.ne	.Lhandler.2
	mov	w0, #42
	b	.Lhandler.3
.Lhandler.2:
	mov	w0, #1
.Lhandler.3:
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	bl	_Exit
.Lhandler.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	handler, .-handler
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
	mov	w0, #10
	str	w0, [x29, #16]
	adrp	x0, handler
	add	x0, x0, :lo12:handler
	str	x0, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	x1, [x29, #24]
	bl	signal
	cmp	x0, #-1
	b.ne	.Lmain.2
	mov	w0, #2
	b	.Lmain.1
.Lmain.2:
	mov	w0, #10
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	raise
	mov	w0, #3
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
