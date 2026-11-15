	.file	"header-stdlib.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	bl	atoi
	cmp	w0, #42
	b.eq	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	mov	w0, #-7
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	bl	abs
	cmp	w0, #7
	b.eq	.Lmain.3
	mov	w0, #2
	b	.Lmain.1
.Lmain.3:
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #32]
	str	xzr, [x29, #40]
	mov	w0, #16
	str	w0, [x29, #48]
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #40]
	ldr	w2, [x29, #48]
	bl	strtol
	cmp	x0, #-255
	b.eq	.Lmain.4
	mov	w0, #3
	b	.Lmain.1
.Lmain.4:
	mov	w0, #0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"42"
.Ls1:
	.string	"-ff"
	.section	.note.GNU-stack,"",@progbits
