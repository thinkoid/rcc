	.file	"string.c"
	.text
	.globl	second
	.type	second, @function
second:
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
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w0, [x0, #1]
	b	.Lsecond.1
.Lsecond.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	second, .-second
	.data
.Ls0:
	.string	"abc"
	.section	.note.GNU-stack,"",@progbits
