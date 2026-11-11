	.file	"conditional.c"
	.text
	.globl	conditional
	.type	conditional, @function
conditional:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	cbz	w0, .Lconditional.2
	ldr	w0, [x29, #20]
	b	.Lconditional.3
.Lconditional.2:
	ldr	w0, [x29, #24]
.Lconditional.3:
	b	.Lconditional.1
.Lconditional.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	conditional, .-conditional
	.section	.note.GNU-stack,"",@progbits
