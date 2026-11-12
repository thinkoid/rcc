	.file	"g17-string-store.c"
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
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	mov	w1, #122
	strb	w1, [x0]
	ldr	x0, [x29, #24]
	ldrb	w0, [x0]
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
	.data
.Ls0:
	.string	"abc"
	.section	.note.GNU-stack,"",@progbits
