	.file	"invariant.c"
	.text
	.globl	invariant
	.type	invariant, @function
invariant:
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
	str	w2, [x29, #24]
	str	wzr, [x29, #28]
	str	wzr, [x29, #32]
.Linvariant.2:
	ldr	w0, [x29, #32]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Linvariant.4
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #20]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	str	w0, [x29, #28]
.Linvariant.3:
	ldr	w0, [x29, #32]
	add	w0, w0, #1
	str	w0, [x29, #32]
	b	.Linvariant.2
.Linvariant.4:
	ldr	w0, [x29, #28]
	b	.Linvariant.1
.Linvariant.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	invariant, .-invariant
	.globl	invariant_constant
	.type	invariant_constant, @function
invariant_constant:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
.Linvariant_constant.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Linvariant_constant.4
	ldr	w0, [x29, #20]
	add	w0, w0, #408
	str	w0, [x29, #20]
.Linvariant_constant.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Linvariant_constant.2
.Linvariant_constant.4:
	ldr	w0, [x29, #20]
	b	.Linvariant_constant.1
.Linvariant_constant.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	invariant_constant, .-invariant_constant
	.section	.note.GNU-stack,"",@progbits
