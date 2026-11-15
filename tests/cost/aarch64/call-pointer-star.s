	.file	"call-pointer-star.c"
	.text
	.globl	plain
	.type	plain, @function
plain:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	ldr	x16, [x29, #32]
	blr	x16
	b	.Lplain.1
.Lplain.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	plain, .-plain
	.globl	star
	.type	star, @function
star:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	ldr	x16, [x29, #32]
	blr	x16
	b	.Lstar.1
.Lstar.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	star, .-star
	.globl	stars
	.type	stars, @function
stars:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	ldr	x16, [x29, #32]
	blr	x16
	b	.Lstars.1
.Lstars.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stars, .-stars
	.section	.note.GNU-stack,"",@progbits
