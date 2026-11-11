	.file	"narrow.c"
	.text
	.globl	f
	.type	f, @function
f:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	strh	w1, [x29, #18]
	ldrb	w0, [x29, #16]
	mov	w1, #4
	sdiv	w0, w0, w1
	ldrsh	w1, [x29, #18]
	add	w0, w0, w1
	b	.Lf.1
.Lf.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	f, .-f
	.globl	g
	.type	g, @function
g:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	add	w0, w0, #1
	b	.Lg.1
.Lg.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	g, .-g
	.globl	h
	.type	h, @function
h:
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
	strb	w0, [x29, #20]
	ldr	w0, [x29, #16]
	strh	w0, [x29, #22]
	ldrb	w0, [x29, #20]
	ldrsh	w1, [x29, #22]
	bl	f
	uxtb	w0, w0
	b	.Lh.1
.Lh.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	h, .-h
	.section	.note.GNU-stack,"",@progbits
