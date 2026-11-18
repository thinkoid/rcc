	.file	"g14-endless-loop.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
.Lmain.2:
	ldr	w0, [x29, #20]
	mov	w1, #2
	udiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cbz	w0, .Lmain.3
	ldr	w0, [x29, #20]
	add	w0, w0, #2
	str	w0, [x29, #20]
	b	.Lmain.2
.Lmain.3:
	mov	w0, #0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
