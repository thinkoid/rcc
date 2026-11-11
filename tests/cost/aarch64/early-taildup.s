	.file	"early-taildup.c"
	.text
	.globl	join
	.type	join, @function
join:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	wzr, [x29, #24]
.Ljoin.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #20]
	cmp	w0, w1
	b.ge	.Ljoin.3
	ldr	w0, [x29, #16]
	cbz	w0, .Ljoin.4
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Ljoin.5
.Ljoin.4:
	ldr	w0, [x29, #24]
	add	w0, w0, #2
	str	w0, [x29, #24]
.Ljoin.5:
	b	.Ljoin.2
.Ljoin.3:
	ldr	w0, [x29, #24]
	b	.Ljoin.1
.Ljoin.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	join, .-join
	.section	.note.GNU-stack,"",@progbits
