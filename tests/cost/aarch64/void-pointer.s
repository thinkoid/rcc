	.file	"void-pointer.c"
	.text
	.globl	round
	.type	round, @function
round:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	b	.Lround.1
.Lround.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	round, .-round
	.section	.note.GNU-stack,"",@progbits
