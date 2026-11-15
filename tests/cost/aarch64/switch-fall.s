	.file	"switch-fall.c"
	.text
	.globl	score
	.type	score, @function
score:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	cmp	w0, #4
	b.hi	.Lscore.4
	adrp	x16, .Lscore.8
	add	x16, x16, :lo12:.Lscore.8
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Lscore.8:
	.quad	.Lscore.2
	.quad	.Lscore.3
	.quad	.Lscore.4
	.quad	.Lscore.5
	.quad	.Lscore.6
	.text
.Lscore.2:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
.Lscore.3:
	ldr	w0, [x29, #20]
	add	w0, w0, #2
	str	w0, [x29, #20]
.Lscore.4:
	ldr	w0, [x29, #20]
	add	w0, w0, #4
	str	w0, [x29, #20]
.Lscore.5:
	ldr	w0, [x29, #20]
	add	w0, w0, #8
	str	w0, [x29, #20]
	b	.Lscore.7
.Lscore.6:
	ldr	w0, [x29, #20]
	add	w0, w0, #16
	str	w0, [x29, #20]
.Lscore.7:
	ldr	w0, [x29, #20]
	b	.Lscore.1
.Lscore.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	score, .-score
	.section	.note.GNU-stack,"",@progbits
