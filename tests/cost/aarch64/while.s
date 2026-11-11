	.file	"while.c"
	.text
	.globl	count
	.type	count, @function
count:
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
.Lcount.2:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lcount.3
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lcount.2
.Lcount.3:
	ldr	w0, [x29, #20]
	b	.Lcount.1
.Lcount.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count, .-count
	.section	.note.GNU-stack,"",@progbits
