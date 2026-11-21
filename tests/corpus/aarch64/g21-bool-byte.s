	.file	"g21-bool-byte.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, b
	add	x0, x0, :lo12:b
	str	x0, [x29, #24]
	str	wzr, [x29, #32]
	ldr	x0, [x29, #24]
	mov	w1, #2
	strb	w1, [x0]
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	mov	w1, #16
	mul	w0, w0, w1
	ldr	w1, [x29, #32]
	add	w1, w1, w0
	str	w1, [x29, #32]
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	cbz	w0, .Lmain.2
	ldr	w0, [x29, #32]
	add	w0, w0, #4
	str	w0, [x29, #32]
.Lmain.2:
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	cmp	w0, #1
	b.eq	.Lmain.3
	ldr	w0, [x29, #32]
	add	w0, w0, #2
	str	w0, [x29, #32]
.Lmain.3:
	adrp	x16, b
	ldrb	w0, [x16, :lo12:b]
	strb	w0, [x29, #16]
	ldrb	w0, [x29, #16]
	sub	w0, w0, #1
	cmp	w0, #0
	cset	w0, ne
	adrp	x16, b
	strb	w0, [x16, :lo12:b]
	ldrb	w0, [x29, #16]
	cmp	w0, #2
	b.eq	.Lmain.4
	mov	w0, #101
	b	.Lmain.1
.Lmain.4:
	ldr	x0, [x29, #24]
	ldrb	w0, [x0]
	ldr	w1, [x29, #32]
	add	w1, w1, w0
	str	w1, [x29, #32]
	ldr	w0, [x29, #32]
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	b
	.type	b, @object
	.size	b, 1
b:
	.zero	1
	.section	.note.GNU-stack,"",@progbits
