	.file	"subscript.c"
	.text
	.globl	subscript
	.type	subscript, @function
subscript:
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
	add	x0, x29, #24
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	str	w2, [x0, w1, sxtw #2]
	add	x0, x29, #24
	ldr	w1, [x29, #16]
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lsubscript.1
.Lsubscript.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	subscript, .-subscript
	.section	.note.GNU-stack,"",@progbits
