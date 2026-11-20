	.file	"header-count.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #112
	.cfi_def_cfa_offset 112
	stp	x29, x30, [sp]
	.cfi_offset w29, -112
	.cfi_offset w30, -104
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #0
	str	w0, [x29, #28]
	str	w0, [x29, #24]
	str	wzr, [x29, #20]
.Lmain.2:
	ldr	w0, [x29, #20]
	cmp	w0, #10
	b.ge	.Lmain.4
	add	x0, x29, #32
	ldr	w1, [x29, #20]
	str	wzr, [x0, w1, sxtw #2]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lmain.2
.Lmain.4:
	str	wzr, [x29, #20]
.Lmain.5:
	adrp	x0, text
	ldr	w1, [x29, #20]
	add	x0, x0, w1, sxtw
	ldrb	w0, [x0, :lo12:text]
	str	w0, [x29, #16]
	cmp	w0, #0
	b.eq	.Lmain.7
	ldr	w0, [x29, #16]
	mov	w1, #3
	udiv	w2, w0, w1
	msub	w0, w2, w1, w0
	adrp	x16, .Lmain.23
	add	x16, x16, :lo12:.Lmain.23
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Lmain.23:
	.quad	.Lmain.24
	.quad	.Lmain.25
	.quad	.Lmain.26
	.text
.Lmain.24:
	cmp	w2, #3
	b.eq	.Lmain.20
	cmp	w2, #16
	b.eq	.Lmain.8
	cmp	w2, #17
	b.eq	.Lmain.11
	cmp	w2, #18
	b.eq	.Lmain.14
	cmp	w2, #19
	b.eq	.Lmain.17
	b	.Lmain.21
.Lmain.25:
	cmp	w2, #3
	b.eq	.Lmain.19
	cmp	w2, #16
	b.eq	.Lmain.9
	cmp	w2, #17
	b.eq	.Lmain.12
	cmp	w2, #18
	b.eq	.Lmain.15
	b	.Lmain.21
.Lmain.26:
	cmp	w2, #10
	b.eq	.Lmain.18
	cmp	w2, #16
	b.eq	.Lmain.10
	cmp	w2, #17
	b.eq	.Lmain.13
	cmp	w2, #18
	b.eq	.Lmain.16
	b	.Lmain.21
.Lmain.8:
.Lmain.9:
.Lmain.10:
.Lmain.11:
.Lmain.12:
.Lmain.13:
.Lmain.14:
.Lmain.15:
.Lmain.16:
.Lmain.17:
	add	x0, x29, #32
	ldr	w1, [x29, #16]
	sub	w1, w1, #48
	ldr	w2, [x0, w1, sxtw #2]
	add	w2, w2, #1
	str	w2, [x0, w1, sxtw #2]
	b	.Lmain.22
.Lmain.18:
.Lmain.19:
.Lmain.20:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lmain.22
.Lmain.21:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lmain.22
.Lmain.22:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lmain.5
.Lmain.7:
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #72]
	ldr	x0, [x29, #72]
	bl	printf
	str	wzr, [x29, #20]
.Lmain.27:
	ldr	w0, [x29, #20]
	cmp	w0, #10
	b.ge	.Lmain.29
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #80]
	add	x0, x29, #32
	ldr	w1, [x29, #20]
	ldr	w0, [x0, w1, sxtw #2]
	str	w0, [x29, #88]
	ldr	x0, [x29, #80]
	ldr	w1, [x29, #88]
	bl	printf
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lmain.27
.Lmain.29:
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	str	x0, [x29, #96]
	ldr	w0, [x29, #24]
	str	w0, [x29, #104]
	ldr	w0, [x29, #28]
	str	w0, [x29, #108]
	ldr	x0, [x29, #96]
	ldr	w1, [x29, #104]
	ldr	w2, [x29, #108]
	bl	printf
	ldr	w0, [x29, #36]
	cmp	w0, #2
	b.ne	.Lmain.31
	ldr	w0, [x29, #40]
	cmp	w0, #5
	b.ne	.Lmain.31
	ldr	w0, [x29, #60]
	cmp	w0, #2
	b.eq	.Lmain.30
.Lmain.31:
	mov	w0, #101
	b	.Lmain.1
.Lmain.30:
	ldr	w0, [x29, #64]
	cmp	w0, #4
	b.ne	.Lmain.33
	ldr	w0, [x29, #68]
	cmp	w0, #2
	b.ne	.Lmain.33
	ldr	w0, [x29, #32]
	cmp	w0, #0
	b.eq	.Lmain.32
.Lmain.33:
	mov	w0, #102
	b	.Lmain.1
.Lmain.32:
	ldr	w0, [x29, #24]
	cmp	w0, #13
	b.eq	.Lmain.34
	mov	w0, #103
	b	.Lmain.1
.Lmain.34:
	ldr	w0, [x29, #28]
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 112
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #112
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"digits ="
.Ls1:
	.string	" %d"
.Ls2:
	.string	", white space = %d, other = %d\n"
	.data
	.type	text, @object
	.size	text, 61
text:
	.string	"In 1978 K&R had 228 pages;\n\tthe 2nd edition, 1988, has 272.\n"
	.section	.note.GNU-stack,"",@progbits
