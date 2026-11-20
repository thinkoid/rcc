	.file	"header-getch.c"
	.text
	.type	read_char, @function
read_char:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, input
	ldr	x0, [x16, :lo12:input]
	ldrb	w0, [x0]
	cmp	w0, #0
	b.eq	.Lread_char.2
	adrp	x16, input
	ldr	x0, [x16, :lo12:input]
	add	x1, x0, #1
	adrp	x16, input
	str	x1, [x16, :lo12:input]
	ldrb	w0, [x0]
	b	.Lread_char.3
.Lread_char.2:
	mov	w0, #-1
.Lread_char.3:
	b	.Lread_char.1
.Lread_char.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read_char, .-read_char
	.globl	getch
	.type	getch, @function
getch:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, bufp
	ldr	w0, [x16, :lo12:bufp]
	cmp	w0, #0
	b.le	.Lgetch.2
	adrp	x0, buf
	adrp	x16, bufp
	ldr	w1, [x16, :lo12:bufp]
	sub	w1, w1, #1
	adrp	x16, bufp
	str	w1, [x16, :lo12:bufp]
	add	x0, x0, w1, sxtw
	ldrb	w0, [x0, :lo12:buf]
	b	.Lgetch.3
.Lgetch.2:
	bl	read_char
.Lgetch.3:
	b	.Lgetch.1
.Lgetch.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	getch, .-getch
	.globl	ungetch
	.type	ungetch, @function
ungetch:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x16, bufp
	ldr	w0, [x16, :lo12:bufp]
	cmp	w0, #100
	b.lt	.Lungetch.2
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	printf
	b	.Lungetch.3
.Lungetch.2:
	adrp	x0, buf
	adrp	x16, bufp
	ldr	w1, [x16, :lo12:bufp]
	add	w2, w1, #1
	adrp	x16, bufp
	str	w2, [x16, :lo12:bufp]
	add	x0, x0, w1, sxtw
	ldr	w1, [x29, #16]
	strb	w1, [x0, :lo12:buf]
.Lungetch.3:
.Lungetch.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	ungetch, .-ungetch
	.globl	getint
	.type	getint, @function
getint:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
.Lgetint.2:
	bl	getch
	str	w0, [x29, #24]
	cmp	w0, #32
	b.eq	.Lgetint.4
	ldr	w0, [x29, #24]
	cmp	w0, #9
	b.eq	.Lgetint.4
	ldr	w0, [x29, #24]
	cmp	w0, #10
	b.ne	.Lgetint.3
.Lgetint.4:
	b	.Lgetint.2
.Lgetint.3:
	ldr	w0, [x29, #24]
	cmp	w0, #-1
	b.ne	.Lgetint.5
	mov	w0, #0
	b	.Lgetint.1
.Lgetint.5:
	ldr	w0, [x29, #24]
	cmp	w0, #45
	b.ne	.Lgetint.6
	mov	w0, #-1
	b	.Lgetint.7
.Lgetint.6:
	mov	w0, #1
.Lgetint.7:
	str	w0, [x29, #28]
	ldr	w0, [x29, #24]
	cmp	w0, #43
	b.eq	.Lgetint.9
	ldr	w0, [x29, #24]
	cmp	w0, #45
	b.ne	.Lgetint.8
.Lgetint.9:
	bl	getch
	str	w0, [x29, #24]
.Lgetint.8:
	ldr	x0, [x29, #16]
	str	wzr, [x0]
.Lgetint.10:
	ldr	w0, [x29, #24]
	cmp	w0, #48
	b.lt	.Lgetint.12
	ldr	w0, [x29, #24]
	cmp	w0, #57
	b.gt	.Lgetint.12
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #16]
	ldr	w1, [x1]
	mov	w2, #10
	mul	w1, w1, w2
	ldr	w2, [x29, #24]
	sub	w2, w2, #48
	add	w1, w1, w2
	str	w1, [x0]
	bl	getch
	str	w0, [x29, #24]
	b	.Lgetint.10
.Lgetint.12:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	ldr	w2, [x0]
	mul	w2, w2, w1
	str	w2, [x0]
	ldr	w0, [x29, #24]
	cmp	w0, #-1
	b.eq	.Lgetint.13
	ldr	w0, [x29, #24]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	ungetch
.Lgetint.13:
	ldr	w0, [x29, #24]
	b	.Lgetint.1
.Lgetint.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	getint, .-getint
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
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
.Lmain.2:
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	getint
	cmp	w0, #0
	b.eq	.Lmain.3
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	add	w1, w1, w0
	str	w1, [x29, #20]
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lmain.2
.Lmain.3:
	mov	w0, #97
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	ungetch
	mov	w0, #98
	str	w0, [x29, #44]
	ldr	w0, [x29, #44]
	bl	ungetch
	mov	w0, #99
	str	w0, [x29, #48]
	ldr	w0, [x29, #48]
	bl	ungetch
	bl	getch
	cmp	w0, #99
	b.ne	.Lmain.5
	bl	getch
	cmp	w0, #98
	b.ne	.Lmain.5
	bl	getch
	cmp	w0, #97
	b.eq	.Lmain.4
.Lmain.5:
	mov	w0, #1
	b	.Lmain.1
.Lmain.4:
	bl	getch
	cmp	w0, #-1
	b.eq	.Lmain.6
	mov	w0, #2
	b	.Lmain.1
.Lmain.6:
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #56]
	ldr	w0, [x29, #24]
	str	w0, [x29, #64]
	ldr	w0, [x29, #20]
	str	w0, [x29, #68]
	ldr	x0, [x29, #56]
	ldr	w1, [x29, #64]
	ldr	w2, [x29, #68]
	bl	printf
	ldr	w0, [x29, #20]
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
	.data
.Ls0:
	.string	"ungetch: too many characters\n"
.Ls1:
	.string	"%d integers, sum %d\n"
.Ls2:
	.string	" 12 -7  +30\t10\n"
	.data
	.type	bufp, @object
	.size	bufp, 4
	.p2align 2
bufp:
	.long	0
	.type	input, @object
	.size	input, 8
	.p2align 3
input:
	.quad	.Ls2
	.bss
	.type	buf, @object
	.size	buf, 100
buf:
	.zero	100
	.section	.note.GNU-stack,"",@progbits
