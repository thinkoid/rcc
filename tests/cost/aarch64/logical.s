	.file	"logical.c"
	.text
	.globl	both
	.type	both, @function
both:
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
	ldr	w0, [x29, #16]
	cbz	w0, .Lboth.2
	ldr	w0, [x29, #20]
	cbz	w0, .Lboth.2
	mov	w0, #1
	b	.Lboth.3
.Lboth.2:
	mov	w0, #0
.Lboth.3:
	b	.Lboth.1
.Lboth.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	either
	.type	either, @function
either:
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
	ldr	w0, [x29, #16]
	cbnz	w0, .Leither.4
	ldr	w0, [x29, #20]
	cbz	w0, .Leither.2
.Leither.4:
	mov	w0, #1
	b	.Leither.3
.Leither.2:
	mov	w0, #0
.Leither.3:
	b	.Leither.1
.Leither.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	either, .-either
	.section	.note.GNU-stack,"",@progbits
