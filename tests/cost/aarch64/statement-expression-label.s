	.file	"statement-expression-label.c"
	.text
	.globl	counted
	.type	counted, @function
counted:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	str	wzr, [x29, #32]
.Lcounted.2:
	ldr	w0, [x29, #20]
	sub	w1, w0, #1
	str	w1, [x29, #20]
	cmp	w0, #0
	b.le	.Lcounted.3
	ldr	w0, [x29, #32]
	add	w0, w0, #1
	str	w0, [x29, #32]
	b	.Lcounted.2
.Lcounted.3:
	ldr	w0, [x29, #32]
	ldr	x1, [x29, #24]
	add	w0, w1, w0
	b	.Lcounted.1
.Lcounted.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	counted, .-counted
	.section	.note.GNU-stack,"",@progbits
