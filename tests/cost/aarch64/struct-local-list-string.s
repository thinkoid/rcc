	.file	"struct-local-list-string.c"
	.text
	.globl	exact
	.type	exact, @function
exact:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	ldrb	w17, [x0]
	strb	w17, [x29, #16]
	ldrb	w17, [x0, #1]
	strb	w17, [x29, #17]
	ldrb	w17, [x0, #2]
	strb	w17, [x29, #18]
	ldrb	w17, [x0, #3]
	strb	w17, [x29, #19]
	mov	w0, #1
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lexact.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	exact, .-exact
	.globl	longer
	.type	longer, @function
longer:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
	str	wzr, [x29, #28]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	ldrb	w17, [x0]
	strb	w17, [x29, #16]
	ldrb	w17, [x0, #1]
	strb	w17, [x29, #17]
	ldrb	w17, [x0, #2]
	strb	w17, [x29, #18]
	ldrb	w17, [x0, #3]
	strb	w17, [x29, #19]
	mov	w0, #1
	str	w0, [x29, #28]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Llonger.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	longer, .-longer
	.data
.Ls0:
	.string	"abc"
.Ls1:
	.string	"abc"
	.section	.note.GNU-stack,"",@progbits
