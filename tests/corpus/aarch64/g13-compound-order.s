	.file	"g13-compound-order.c"
	.text
	.globl	bump
	.type	bump, @function
bump:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	mov	w1, #10
	str	w1, [x0]
	mov	w0, #1
	b	.Lbump.1
.Lbump.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	bump, .-bump
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -64
	mov	w0, #1
	str	w0, [x29, #24]
	mov	w0, #1
	str	w0, [x29, #28]
	mov	w0, #1
	str	w0, [x29, #32]
	mov	w0, #1
	str	w0, [x29, #36]
	mov	w19, #1
	add	x0, x29, #24
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	bump
	ldr	w1, [x29, #24]
	add	w1, w1, w0
	str	w1, [x29, #24]
	add	x0, x29, #28
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	bl	bump
	ldr	w1, [x29, #28]
	add	w1, w1, w0
	str	w1, [x29, #28]
	str	w1, [x29, #40]
	add	x0, x29, #32
	str	x0, [x29, #64]
	ldr	x0, [x29, #64]
	bl	bump
	ldr	w1, [x29, #32]
	mul	w1, w1, w0
	str	w1, [x29, #32]
	mov	w0, #2
	str	w0, [x29, #36]
	ldr	w1, [x29, #36]
	add	w1, w1, w0
	str	w1, [x29, #36]
	mov	w19, #2
	add	w19, w19, w19
	ldr	w0, [x29, #24]
	cmp	w0, #11
	cset	w0, eq
	ldr	w1, [x29, #28]
	cmp	w1, #11
	cset	w1, eq
	mov	w2, #2
	mul	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #40]
	cmp	w1, #11
	cset	w1, eq
	mov	w2, #4
	mul	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #32]
	cmp	w1, #10
	cset	w1, eq
	mov	w2, #8
	mul	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #36]
	cmp	w1, #4
	cset	w1, eq
	mov	w2, #16
	mul	w1, w1, w2
	add	w0, w0, w1
	cmp	w19, #4
	cset	w1, eq
	mov	w2, #32
	mul	w1, w1, w2
	add	w0, w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 80
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
