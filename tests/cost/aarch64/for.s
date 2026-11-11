	.file	"for.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
.Lsum.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lsum.4
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	str	w0, [x29, #20]
.Lsum.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lsum.2
.Lsum.4:
	ldr	w0, [x29, #20]
	b	.Lsum.1
.Lsum.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
