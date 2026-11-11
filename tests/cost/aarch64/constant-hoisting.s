	.file	"constant-hoisting.c"
	.text
	.globl	two_constants
	.type	two_constants, @function
two_constants:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	mov	x1, #52719
	movk	x1, #0x89ab, lsl #16
	movk	x1, #0x4567, lsl #32
	movk	x1, #0x123, lsl #48
	add	x0, x0, x1
	ldr	x1, [x29, #24]
	add	x0, x0, x1
	mov	x1, #52727
	movk	x1, #0x89ab, lsl #16
	movk	x1, #0x4567, lsl #32
	movk	x1, #0x123, lsl #48
	add	x0, x0, x1
	b	.Ltwo_constants.1
.Ltwo_constants.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	two_constants, .-two_constants
	.section	.note.GNU-stack,"",@progbits
