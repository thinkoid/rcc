	.file	"bit-field-store-volatile-address.c"
	.text
	.globl	store
	.type	store, @function
store:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x16, d
	ldr	x0, [x16, :lo12:d]
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #16]
	lsl	w1, w1, #3
	ldr	x2, [x29, #24]
	ldrsb	w2, [x2]
	and	w2, w2, #0xffffff07
	orr	w1, w1, w2
	strb	w1, [x0]
.Lstore.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	store, .-store
	.bss
	.globl	d
	.type	d, @object
	.size	d, 8
	.p2align 3
d:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
