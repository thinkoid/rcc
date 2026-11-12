	.file	"g6-ptr-wrap.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #24]
	str	x0, [x29, #32]
	mov	x0, #-1
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	ldr	w1, [x29, #16]
	add	w1, w1, #2
	lsr	x0, x0, x1
	str	x0, [x29, #48]
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #32]
	ldr	x2, [x29, #48]
	add	x1, x1, x2, lsl #3
	cmp	x0, x1
	cset	w0, lo
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
