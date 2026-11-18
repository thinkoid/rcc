	.file	"header-regex.c"
	.text
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
	str	x1, [x29, #24]
	str	wzr, [x29, #32]
	str	wzr, [x29, #36]
.Lsum.2:
	ldr	w0, [x29, #36]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lsum.4
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #36]
	ldr	w0, [x0, w1, sxtw #2]
	ldr	w1, [x29, #32]
	add	w1, w1, w0
	str	w1, [x29, #32]
	ldr	w0, [x29, #36]
	add	w0, w0, #1
	str	w0, [x29, #36]
	b	.Lsum.2
.Lsum.4:
	ldr	w0, [x29, #32]
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
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #24]
	mov	w0, #3
	str	w0, [x29, #28]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	w0, [x29, #28]
	ldr	x1, [x29, #32]
	bl	sum
	cmp	w0, #6
	b.eq	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	mov	w0, #0
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
	.section	.note.GNU-stack,"",@progbits
