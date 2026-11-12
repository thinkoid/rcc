	.file	"register-remainder.c"
	.text
	.globl	rem
	.type	rem, @function
rem:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	mov	w19, w0
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	sdiv	w1, w19, w0
	msub	w0, w1, w0, w19
	b	.Lrem.1
.Lrem.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem, .-rem
	.globl	urem
	.type	urem, @function
urem:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	mov	w19, w0
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	udiv	w1, w19, w0
	msub	w0, w1, w0, w19
	b	.Lurem.1
.Lurem.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	urem, .-urem
	.section	.note.GNU-stack,"",@progbits
