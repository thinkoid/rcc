	.file	"bool-postfix-side-effect.c"
	.text
	.globl	take
	.type	take, @function
take:
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
	add	x1, x0, #1
	adrp	x16, cursor
	str	x1, [x16, :lo12:cursor]
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	add	w1, w1, #1
	cmp	w1, #0
	cset	w1, ne
	strb	w1, [x0]
	ldrb	w0, [x29, #24]
	b	.Ltake.1
.Ltake.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	take, .-take
	.bss
	.globl	cursor
	.type	cursor, @object
	.size	cursor, 8
	.p2align 3
cursor:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
