	.file	"bit-field-compound-volatile.c"
	.text
	.globl	add
	.type	add, @function
add:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x16, v
	ldr	w0, [x16, :lo12:v]
	str	w0, [x29, #28]
	ldr	x0, [x29, #16]
	ldrsb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrsb	w1, [x29, #24]
	asr	w1, w1, #3
	ldr	w2, [x29, #28]
	add	w1, w1, w2
	lsl	w1, w1, #3
	ldrsb	w2, [x29, #24]
	and	w2, w2, #0xffffff07
	orr	w1, w1, w2
	strb	w1, [x0]
.Ladd.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add, .-add
	.bss
	.globl	v
	.type	v, @object
	.size	v, 4
	.p2align 2
v:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
