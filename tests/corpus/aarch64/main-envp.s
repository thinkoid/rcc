	.file	"main-envp.c"
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
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	wzr, [x29, #40]
	ldr	w0, [x29, #16]
	cmp	w0, #1
	b.ne	.Lmain.3
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #16]
	ldr	x0, [x0, w1, sxtw #3]
	cmp	x0, #0
	b.eq	.Lmain.2
.Lmain.3:
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	ldr	x0, [x29, #32]
	adrp	x1, :got:environ
	ldr	x1, [x1, :got_lo12:environ]
	ldr	x1, [x1]
	cmp	x0, x1
	b.eq	.Lmain.4
	mov	w0, #102
	b	.Lmain.1
.Lmain.4:
.Lmain.5:
	ldr	x0, [x29, #32]
	ldr	w1, [x29, #40]
	ldr	x0, [x0, w1, sxtw #3]
	cmp	x0, #0
	b.eq	.Lmain.6
	ldr	w0, [x29, #40]
	add	w0, w0, #1
	str	w0, [x29, #40]
	b	.Lmain.5
.Lmain.6:
	adrp	x0, :got:environ
	ldr	x0, [x0, :got_lo12:environ]
	ldr	x0, [x0]
	ldr	w1, [x29, #40]
	ldr	x0, [x0, w1, sxtw #3]
	cmp	x0, #0
	b.eq	.Lmain.7
	mov	w0, #103
	b	.Lmain.1
.Lmain.7:
	mov	w0, #0
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
