	.file	"block-order.c"
	.text
	.globl	block_order
	.type	block_order, @function
block_order:
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
	str	wzr, [x29, #24]
.Lblock_order.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #20]
	cmp	w0, w1
	b.ge	.Lblock_order.3
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ne	.Lblock_order.4
	mov	w0, #1
	b	.Lblock_order.1
.Lblock_order.4:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lblock_order.2
.Lblock_order.3:
	mov	w0, #0
	b	.Lblock_order.1
.Lblock_order.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	block_order, .-block_order
	.section	.note.GNU-stack,"",@progbits
