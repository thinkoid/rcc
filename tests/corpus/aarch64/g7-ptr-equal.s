	.file	"g7-ptr-equal.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	add	x0, x29, #20
	add	x0, x0, #4
	add	x1, x29, #24
	cmp	x0, x1
	cset	w0, eq
	add	x1, x29, #24
	add	x1, x1, #4
	add	x2, x29, #20
	cmp	x1, x2
	cset	w1, eq
	add	w0, w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
