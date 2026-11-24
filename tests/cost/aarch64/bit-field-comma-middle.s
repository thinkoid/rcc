	.file	"bit-field-comma-middle.c"
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
	str	x0, [x29, #16]
	adrp	x16, r
	ldr	w0, [x16, :lo12:r]
	adrp	x16, g
	str	w0, [x16, :lo12:g]
	ldr	x0, [x29, #16]
	ldrb	w1, [x0, #1]
	orr	w1, w1, #0x1
	strb	w1, [x0, #1]
	adrp	x16, r
	ldr	w0, [x16, :lo12:r]
	adrp	x16, g
	str	w0, [x16, :lo12:g]
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
	.globl	set
	.type	set, @function
set:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x16, r
	ldr	w0, [x16, :lo12:r]
	adrp	x16, g
	str	w0, [x16, :lo12:g]
	ldr	x0, [x29, #16]
	adrp	x16, r
	ldr	w1, [x16, :lo12:r]
	lsl	w1, w1, #3
	ldr	x2, [x29, #16]
	ldrsb	w2, [x2]
	and	w2, w2, #0xffffff07
	orr	w1, w1, w2
	strb	w1, [x0]
	adrp	x16, r
	ldr	w0, [x16, :lo12:r]
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lset.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	set, .-set
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	r
	.type	r, @object
	.size	r, 4
	.p2align 2
r:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
