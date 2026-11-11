	.file	"mul-three.c"
	.text
	.globl	mul_three
	.type	mul_three, @function
mul_three:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	mov	w1, #3
	mul	w0, w0, w1
	b	.Lmul_three.1
.Lmul_three.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mul_three, .-mul_three
	.section	.note.GNU-stack,"",@progbits
