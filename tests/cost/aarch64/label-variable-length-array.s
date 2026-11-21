	.file	"label-variable-length-array.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
.Lf.2:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	ldrsw	x0, [x29, #16]
	str	x0, [x29, #24]
	mov	x17, sp
	str	x17, [x29, #32]
	ldr	x0, [x29, #24]
	sub	x0, sp, x0
	and	x0, x0, #0xfffffffffffffff0
	mov	sp, x0
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	mov	w1, #1
	strb	w1, [x0]
	ldr	w0, [x29, #20]
	cmp	w0, #2
	b.ge	.Lf.3
	ldr	x17, [x29, #32]
	mov	sp, x17
	b	.Lf.2
.Lf.3:
	ldr	x0, [x29, #40]
	ldrb	w0, [x0]
	b	.Lf.1
.Lf.1:
	mov	sp, x29
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	f, .-f
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
	mov	w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	bl	f
	cmp	w0, #1
	cset	w0, ne
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
