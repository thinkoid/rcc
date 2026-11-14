	.file	"header-stdio.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #128
	.cfi_def_cfa_offset 128
	stp	x29, x30, [sp]
	.cfi_offset w29, -128
	.cfi_offset w30, -120
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	str	x0, [x29, #56]
	mov	x0, #32
	str	x0, [x29, #64]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #72]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #80]
	mov	w0, #42
	str	w0, [x29, #88]
	ldr	x0, [x29, #56]
	ldr	x1, [x29, #64]
	ldr	x2, [x29, #72]
	ldr	x3, [x29, #80]
	ldr	w4, [x29, #88]
	bl	snprintf
	str	w0, [x29, #48]
	ldr	w0, [x29, #48]
	cmp	w0, #15
	b.eq	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	add	x0, x29, #16
	str	x0, [x29, #96]
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	str	x0, [x29, #104]
	ldr	x0, [x29, #96]
	ldr	x1, [x29, #104]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.3
	mov	w0, #2
	b	.Lmain.1
.Lmain.3:
	adrp	x0, .Ls3
	add	x0, x0, :lo12:.Ls3
	str	x0, [x29, #112]
	add	x0, x29, #16
	str	x0, [x29, #120]
	ldr	x0, [x29, #112]
	ldr	x1, [x29, #120]
	bl	printf
	mov	w0, #0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 128
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #128
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"hello, %s %d"
.Ls1:
	.string	"world"
.Ls2:
	.string	"hello, world 42"
.Ls3:
	.string	"%s\n"
	.section	.note.GNU-stack,"",@progbits
