	.file	"typedef.c"
	.text
	.globl	length
	.type	length, @function
length:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	xzr, [x29, #24]
.Llength.2:
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	ldrb	w0, [x0, x1]
	cbz	w0, .Llength.3
	ldr	x0, [x29, #24]
	add	x0, x0, #1
	str	x0, [x29, #24]
	b	.Llength.2
.Llength.3:
	ldr	x0, [x29, #24]
	b	.Llength.1
.Llength.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	length, .-length
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	add	x0, x29, #20
	str	x0, [x29, #40]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	mov	w0, #8
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	ldr	x1, [x29, #40]
	ldrb	w1, [x1]
	add	w0, w0, w1
	b	.Lsum.1
.Lsum.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
