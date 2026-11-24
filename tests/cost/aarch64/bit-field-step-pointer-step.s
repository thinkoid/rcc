	.file	"bit-field-step-pointer-step.c"
	.text
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, cursor
	ldr	x0, [x16, :lo12:cursor]
	add	x1, x0, #8
	adrp	x16, cursor
	str	x1, [x16, :lo12:cursor]
	add	x0, x0, #4
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldrb	w0, [x0]
	strb	w0, [x29, #16]
	ldr	x0, [x29, #24]
	ldrb	w1, [x29, #16]
	lsl	w1, w1, #28
	lsr	w1, w1, #29
	add	w1, w1, #1
	lsl	w1, w1, #1
	and	w1, w1, #0xe
	ldrb	w2, [x29, #16]
	and	w2, w2, #0xfffffff1
	orr	w1, w1, w2
	strb	w1, [x0]
	ldrb	w0, [x29, #16]
	lsl	w0, w0, #28
	lsr	w0, w0, #29
	b	.Lnext.1
.Lnext.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	next, .-next
	.bss
	.globl	cursor
	.type	cursor, @object
	.size	cursor, 8
	.p2align 3
cursor:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
