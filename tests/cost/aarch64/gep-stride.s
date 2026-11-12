	.file	"gep-stride.c"
	.text
	.globl	row
	.type	row, @function
row:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	add	x0, x29, #20
	ldr	w1, [x29, #16]
	sxtw	x1, w1
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	b	.Lrow.1
.Lrow.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row, .-row
	.section	.note.GNU-stack,"",@progbits
