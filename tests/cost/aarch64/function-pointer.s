	.file	"function-pointer.c"
	.text
	.globl	swap
	.type	swap, @function
swap:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	str	w0, [x29, #40]
	ldr	x0, [x29, #24]
	str	x0, [x29, #48]
	ldr	w0, [x29, #40]
	ldr	x1, [x29, #48]
	bl	install
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #24]
	cmp	x0, x1
	b.eq	.Lswap.3
	ldr	x0, [x29, #32]
	cbnz	x0, .Lswap.2
.Lswap.3:
	mov	x0, #0
	b	.Lswap.1
.Lswap.2:
	ldr	x0, [x29, #32]
	b	.Lswap.1
.Lswap.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	swap, .-swap
	.section	.note.GNU-stack,"",@progbits
