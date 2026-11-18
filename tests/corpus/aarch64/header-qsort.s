	.file	"header-qsort.c"
	.text
	.globl	compare
	.type	compare, @function
compare:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #32]
	ldr	w0, [x0]
	ldr	x1, [x29, #40]
	ldr	w1, [x1]
	cmp	w0, w1
	cset	w0, gt
	ldr	x1, [x29, #32]
	ldr	w1, [x1]
	ldr	x2, [x29, #40]
	ldr	w2, [x2]
	cmp	w1, w2
	cset	w1, lt
	sub	w0, w0, w1
	b	.Lcompare.1
.Lcompare.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compare, .-compare
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
	mov	w0, #42
	str	w0, [x29, #16]
	mov	w0, #-7
	str	w0, [x29, #20]
	mov	w0, #1000
	str	w0, [x29, #24]
	str	wzr, [x29, #28]
	mov	w0, #-7
	str	w0, [x29, #32]
	mov	w0, #3
	str	w0, [x29, #36]
	add	x0, x29, #16
	str	x0, [x29, #48]
	mov	x0, #6
	str	x0, [x29, #56]
	mov	x0, #4
	str	x0, [x29, #64]
	adrp	x0, compare
	add	x0, x0, :lo12:compare
	str	x0, [x29, #72]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #56]
	ldr	x2, [x29, #64]
	ldr	x3, [x29, #72]
	bl	qsort
	mov	w0, #1
	str	w0, [x29, #40]
.Lmain.2:
	ldr	w0, [x29, #40]
	cmp	w0, #6
	b.ge	.Lmain.4
	add	x0, x29, #16
	ldr	w1, [x29, #40]
	sub	w1, w1, #1
	ldr	w0, [x0, w1, sxtw #2]
	add	x1, x29, #16
	ldr	w2, [x29, #40]
	ldr	w1, [x1, w2, sxtw #2]
	cmp	w0, w1
	b.le	.Lmain.5
	mov	w0, #1
	b	.Lmain.1
.Lmain.5:
	ldr	w0, [x29, #40]
	add	w0, w0, #1
	str	w0, [x29, #40]
	b	.Lmain.2
.Lmain.4:
	mov	w0, #0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 80
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
