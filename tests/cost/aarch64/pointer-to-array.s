	.file	"pointer-to-array.c"
	.text
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	w2, [x29, #28]
	ldr	x0, [x29, #16]
	ldrsw	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w1, [x29, #28]
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lelement.1
.Lelement.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element, .-element
	.section	.note.GNU-stack,"",@progbits
