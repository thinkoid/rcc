	.file	"register-blocks.c"
	.text
	.globl	blocks
	.type	blocks, @function
blocks:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	stp	x21, x22, [x29, #32]
	.cfi_offset w21, -16
	.cfi_offset w22, -8
	mov	w19, w0
	mov	w20, #0
	mov	w0, #2
	mul	w21, w19, w0
	add	w20, w20, w21
	mov	w0, #3
	mul	w21, w19, w0
	add	w20, w20, w21
	add	w22, w20, #1
	mov	w0, w22
	b	.Lblocks.1
.Lblocks.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x21, x22, [x29, #32]
	.cfi_restore w21
	.cfi_restore w22
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	blocks, .-blocks
	.section	.note.GNU-stack,"",@progbits
