	.file	"header-squeeze.c"
	.text
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
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	strlen
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
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #128
	.cfi_def_cfa_offset 128
	stp	x29, x30, [sp]
	.cfi_offset w29, -128
	.cfi_offset w30, -120
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, text
	add	x0, x0, :lo12:text
	str	x0, [x29, #24]
	mov	w0, #97
	str	w0, [x29, #32]
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	bl	squeeze
	adrp	x0, text
	add	x0, x0, :lo12:text
	str	x0, [x29, #40]
	adrp	x0, tail
	add	x0, x0, :lo12:tail
	str	x0, [x29, #48]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #48]
	bl	concat
	adrp	x0, text
	add	x0, x0, :lo12:text
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	bl	strlen
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	sub	w0, w0, #100
	str	w0, [x29, #64]
	ldr	w0, [x29, #64]
	bl	abs
	str	w0, [x29, #16]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #72]
	adrp	x0, text
	add	x0, x0, :lo12:text
	str	x0, [x29, #80]
	ldr	w0, [x29, #16]
	str	w0, [x29, #88]
	ldr	x0, [x29, #72]
	ldr	x1, [x29, #80]
	ldr	w2, [x29, #88]
	bl	printf
	adrp	x0, text
	add	x0, x0, :lo12:text
	str	x0, [x29, #96]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #104]
	ldr	x0, [x29, #96]
	ldr	x1, [x29, #104]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.2
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	adrp	x0, text
	add	x0, x0, :lo12:text
	str	x0, [x29, #112]
	ldr	x0, [x29, #112]
	bl	count
	cmp	w0, #15
	b.ne	.Lmain.4
	adrp	x0, tail
	add	x0, x0, :lo12:tail
	str	x0, [x29, #120]
	ldr	x0, [x29, #120]
	bl	count
	cmp	w0, #7
	b.eq	.Lmain.3
.Lmain.4:
	mov	w0, #102
	b	.Lmain.1
.Lmain.3:
	ldr	w0, [x29, #16]
	cmp	w0, #85
	b.eq	.Lmain.5
	mov	w0, #103
	b	.Lmain.1
.Lmain.5:
	ldr	w0, [x29, #16]
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 128
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #128
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.globl	squeeze
	.type	squeeze, @function
squeeze:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	mov	w0, #0
	str	w0, [x29, #32]
	str	w0, [x29, #28]
.Lsqueeze.2:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #0
	b.eq	.Lsqueeze.4
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	ldrb	w0, [x0, w1, sxtw]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.eq	.Lsqueeze.5
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #32]
	add	w2, w1, #1
	str	w2, [x29, #32]
	ldr	x2, [x29, #16]
	ldr	w3, [x29, #28]
	ldrb	w2, [x2, w3, sxtw]
	strb	w2, [x0, w1, sxtw]
.Lsqueeze.5:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lsqueeze.2
.Lsqueeze.4:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #32]
	strb	wzr, [x0, w1, sxtw]
.Lsqueeze.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	squeeze, .-squeeze
	.globl	concat
	.type	concat, @function
concat:
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
	mov	w0, #0
	str	w0, [x29, #36]
	str	w0, [x29, #32]
.Lconcat.2:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #32]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #0
	b.eq	.Lconcat.3
	ldr	w0, [x29, #32]
	add	w0, w0, #1
	str	w0, [x29, #32]
	b	.Lconcat.2
.Lconcat.3:
.Lconcat.4:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #32]
	add	w2, w1, #1
	str	w2, [x29, #32]
	ldr	x2, [x29, #24]
	ldr	w3, [x29, #36]
	add	w4, w3, #1
	str	w4, [x29, #36]
	ldrb	w2, [x2, w3, sxtw]
	strb	w2, [x0, w1, sxtw]
	cmp	w2, #0
	b.eq	.Lconcat.5
	b	.Lconcat.4
.Lconcat.5:
.Lconcat.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	concat, .-concat
	.data
.Ls0:
	.string	"%s %d\n"
.Ls1:
	.string	"bnn bndn cabana"
	.data
	.globl	text
	.type	text, @object
	.size	text, 64
text:
	.string	"banana bandana"
	.zero	49
	.globl	tail
	.type	tail, @object
	.size	tail, 8
tail:
	.string	" cabana"
	.section	.note.GNU-stack,"",@progbits
