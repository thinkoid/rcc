	.file	"array-local-string.c"
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
	.globl	unsized
	.type	unsized, @function
unsized:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	adrp	x1, .Ls1
	add	x1, x1, :lo12:.Ls1
	mov	x16, #17
.Lunsized.2:
	ldrb	w17, [x1], #1
	strb	w17, [x0], #1
	subs	x16, x16, #1
	b.ne	.Lunsized.2
	add	x0, x29, #16
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	use
.Lunsized.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsized, .-unsized
	.globl	padded
	.type	padded, @function
padded:
	.cfi_startproc
	sub	sp, sp, #128
	.cfi_def_cfa_offset 128
	stp	x29, x30, [sp]
	.cfi_offset w29, -128
	.cfi_offset w30, -120
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	ldrb	w17, [x0]
	strb	w17, [x29, #16]
	ldrb	w17, [x0, #1]
	strb	w17, [x29, #17]
	ldrb	w17, [x0, #2]
	strb	w17, [x29, #18]
	ldrb	w17, [x0, #3]
	strb	w17, [x29, #19]
	add	x0, x29, #20
	mov	x16, #96
.Lpadded.2:
	strb	wzr, [x0], #1
	subs	x16, x16, #1
	b.ne	.Lpadded.2
	add	x0, x29, #16
	str	x0, [x29, #120]
	ldr	x0, [x29, #120]
	bl	use
.Lpadded.1:
	.cfi_def_cfa wsp, 128
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #128
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	padded, .-padded
	.globl	braced
	.type	braced, @function
braced:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls3
	add	x0, x0, :lo12:.Ls3
	ldrb	w17, [x0]
	strb	w17, [x29, #16]
	ldrb	w17, [x0, #1]
	strb	w17, [x29, #17]
	ldrb	w17, [x0, #2]
	strb	w17, [x29, #18]
	ldrb	w17, [x0, #3]
	strb	w17, [x29, #19]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lbraced.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	braced, .-braced
	.globl	hundred
	.type	hundred, @function
hundred:
	.cfi_startproc
	sub	sp, sp, #128
	.cfi_def_cfa_offset 128
	stp	x29, x30, [sp]
	.cfi_offset w29, -128
	.cfi_offset w30, -120
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	adrp	x1, .Ls4
	add	x1, x1, :lo12:.Ls4
	mov	x16, #100
.Lhundred.2:
	ldrb	w17, [x1], #1
	strb	w17, [x0], #1
	subs	x16, x16, #1
	b.ne	.Lhundred.2
	add	x0, x29, #16
	str	x0, [x29, #120]
	ldr	x0, [x29, #120]
	bl	use
.Lhundred.1:
	.cfi_def_cfa wsp, 128
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #128
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	hundred, .-hundred
	.data
.Ls0:
	.string	"abc"
.Ls1:
	.string	"a longer message"
.Ls2:
	.string	"abc"
.Ls3:
	.string	"abc"
.Ls4:
	.string	"012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"
	.section	.note.GNU-stack,"",@progbits
