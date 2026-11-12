	.file	"g12-block-slot.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	mov	w0, #4
	ldr	w1, [x29, #16]
	add	w0, w0, w1
	str	w0, [x29, #32]
	add	x0, x29, #32
	str	x0, [x29, #24]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #40]
	ldr	x0, [x29, #24]
	str	x0, [x29, #48]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #48]
	bl	printf
	mov	w0, #9
	str	w0, [x29, #56]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #64]
	add	x0, x29, #56
	str	x0, [x29, #72]
	ldr	x0, [x29, #64]
	ldr	x1, [x29, #72]
	bl	printf
	ldr	x0, [x29, #24]
	ldr	w0, [x0]
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 80
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"%p\n"
.Ls1:
	.string	"%p\n"
	.section	.note.GNU-stack,"",@progbits
