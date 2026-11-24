	.file	"bool-compound-volatile-address.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x16, p
	ldr	x0, [x16, :lo12:p]
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #24]
	ldrb	w1, [x1]
	ldr	w2, [x29, #16]
	orr	w1, w1, w2
	cmp	w1, #0
	cset	w1, ne
	strb	w1, [x0]
.Lmark.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mark, .-mark
	.bss
	.globl	p
	.type	p, @object
	.size	p, 8
	.p2align 3
p:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
