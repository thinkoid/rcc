	.file	"header-itoa.c"
	.text
	.globl	reverse
	.type	reverse, @function
reverse:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	wzr, [x29, #28]
	ldr	x0, [x29, #16]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	strlen
	sub	x0, x0, #1
	str	w0, [x29, #32]
.Lreverse.2:
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #32]
	cmp	w0, w1
	b.ge	.Lreverse.4
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	ldrb	w0, [x0, w1, sxtw]
	str	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #28]
	ldr	x2, [x29, #16]
	ldr	w3, [x29, #32]
	ldrb	w2, [x2, w3, sxtw]
	strb	w2, [x0, w1, sxtw]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #32]
	ldr	w2, [x29, #24]
	strb	w2, [x0, w1, sxtw]
.Lreverse.3:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	ldr	w0, [x29, #32]
	sub	w0, w0, #1
	str	w0, [x29, #32]
	b	.Lreverse.2
.Lreverse.4:
.Lreverse.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	reverse, .-reverse
	.globl	itoa
	.type	itoa, @function
itoa:
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
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	cmp	w0, #0
	b.ge	.Litoa.2
	ldr	w0, [x29, #16]
	neg	w0, w0
	str	w0, [x29, #16]
.Litoa.2:
	str	wzr, [x29, #32]
.Litoa.3:
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	add	w2, w1, #1
	str	w2, [x29, #32]
	ldr	w2, [x29, #16]
	mov	w3, #10
	sdiv	w4, w2, w3
	msub	w2, w4, w3, w2
	add	w2, w2, #48
	strb	w2, [x0, w1, sxtw]
.Litoa.4:
	mov	w0, #10
	ldr	w1, [x29, #16]
	sdiv	w1, w1, w0
	str	w1, [x29, #16]
	cmp	w1, #0
	b.gt	.Litoa.3
	ldr	w0, [x29, #36]
	cmp	w0, #0
	b.ge	.Litoa.6
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	add	w2, w1, #1
	str	w2, [x29, #32]
	mov	w2, #45
	strb	w2, [x0, w1, sxtw]
.Litoa.6:
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	strb	wzr, [x0, w1, sxtw]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	reverse
.Litoa.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	itoa, .-itoa
	.globl	trim
	.type	trim, @function
trim:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	strlen
	sub	x0, x0, #1
	str	w0, [x29, #24]
.Ltrim.2:
	ldr	w0, [x29, #24]
	cmp	w0, #0
	b.lt	.Ltrim.4
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #32
	b.eq	.Ltrim.5
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #9
	b.eq	.Ltrim.5
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #10
	b.ne	.Ltrim.4
.Ltrim.5:
.Ltrim.3:
	ldr	w0, [x29, #24]
	sub	w0, w0, #1
	str	w0, [x29, #24]
	b	.Ltrim.2
.Ltrim.4:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	add	w1, w1, #1
	strb	wzr, [x0, w1, sxtw]
	ldr	w0, [x29, #24]
	b	.Ltrim.1
.Ltrim.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	trim, .-trim
	.globl	count_digits
	.type	count_digits, @function
count_digits:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	wzr, [x29, #28]
	str	wzr, [x29, #24]
.Lcount_digits.2:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #0
	b.eq	.Lcount_digits.4
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #48
	b.lt	.Lcount_digits.3
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #57
	b.gt	.Lcount_digits.3
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
.Lcount_digits.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lcount_digits.2
.Lcount_digits.4:
	ldr	w0, [x29, #28]
	b	.Lcount_digits.1
.Lcount_digits.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count_digits, .-count_digits
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #416
	.cfi_def_cfa_offset 416
	stp	x29, x30, [sp]
	.cfi_offset w29, -416
	.cfi_offset w30, -408
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #64]
	add	x0, x29, #16
	str	x0, [x29, #72]
	ldr	w0, [x29, #64]
	ldr	x1, [x29, #72]
	bl	itoa
	add	x0, x29, #16
	str	x0, [x29, #80]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #88]
	ldr	x0, [x29, #80]
	ldr	x1, [x29, #88]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	mov	w0, #7
	str	w0, [x29, #96]
	add	x0, x29, #16
	str	x0, [x29, #104]
	ldr	w0, [x29, #96]
	ldr	x1, [x29, #104]
	bl	itoa
	add	x0, x29, #16
	str	x0, [x29, #112]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #120]
	ldr	x0, [x29, #112]
	ldr	x1, [x29, #120]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.3
	mov	w0, #2
	b	.Lmain.1
.Lmain.3:
	mov	w0, #-7
	str	w0, [x29, #128]
	add	x0, x29, #16
	str	x0, [x29, #136]
	ldr	w0, [x29, #128]
	ldr	x1, [x29, #136]
	bl	itoa
	add	x0, x29, #16
	str	x0, [x29, #144]
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	str	x0, [x29, #152]
	ldr	x0, [x29, #144]
	ldr	x1, [x29, #152]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.4
	mov	w0, #3
	b	.Lmain.1
.Lmain.4:
	mov	w0, #1234
	str	w0, [x29, #160]
	add	x0, x29, #16
	str	x0, [x29, #168]
	ldr	w0, [x29, #160]
	ldr	x1, [x29, #168]
	bl	itoa
	add	x0, x29, #16
	str	x0, [x29, #176]
	adrp	x0, .Ls3
	add	x0, x0, :lo12:.Ls3
	str	x0, [x29, #184]
	ldr	x0, [x29, #176]
	ldr	x1, [x29, #184]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.5
	mov	w0, #4
	b	.Lmain.1
.Lmain.5:
	mov	w0, #32307
	movk	w0, #0xfffe, lsl #16
	str	w0, [x29, #192]
	add	x0, x29, #32
	str	x0, [x29, #200]
	ldr	w0, [x29, #192]
	ldr	x1, [x29, #200]
	bl	itoa
	add	x0, x29, #32
	str	x0, [x29, #208]
	adrp	x0, .Ls4
	add	x0, x0, :lo12:.Ls4
	str	x0, [x29, #216]
	ldr	x0, [x29, #208]
	ldr	x1, [x29, #216]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.6
	mov	w0, #5
	b	.Lmain.1
.Lmain.6:
	mov	w0, #2147483647
	str	w0, [x29, #224]
	add	x0, x29, #48
	str	x0, [x29, #232]
	ldr	w0, [x29, #224]
	ldr	x1, [x29, #232]
	bl	itoa
	add	x0, x29, #48
	str	x0, [x29, #240]
	adrp	x0, .Ls5
	add	x0, x0, :lo12:.Ls5
	str	x0, [x29, #248]
	ldr	x0, [x29, #240]
	ldr	x1, [x29, #248]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.7
	mov	w0, #6
	b	.Lmain.1
.Lmain.7:
	adrp	x0, abc
	add	x0, x0, :lo12:abc
	str	x0, [x29, #256]
	ldr	x0, [x29, #256]
	bl	reverse
	adrp	x0, abc
	add	x0, x0, :lo12:abc
	str	x0, [x29, #264]
	adrp	x0, .Ls6
	add	x0, x0, :lo12:.Ls6
	str	x0, [x29, #272]
	ldr	x0, [x29, #264]
	ldr	x1, [x29, #272]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.8
	mov	w0, #7
	b	.Lmain.1
.Lmain.8:
	adrp	x0, empty
	add	x0, x0, :lo12:empty
	str	x0, [x29, #280]
	ldr	x0, [x29, #280]
	bl	reverse
	adrp	x0, empty
	add	x0, x0, :lo12:empty
	str	x0, [x29, #288]
	adrp	x0, .Ls7
	add	x0, x0, :lo12:.Ls7
	str	x0, [x29, #296]
	ldr	x0, [x29, #288]
	ldr	x1, [x29, #296]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.9
	mov	w0, #8
	b	.Lmain.1
.Lmain.9:
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #304]
	ldr	x0, [x29, #304]
	bl	trim
	cmp	w0, #4
	b.ne	.Lmain.11
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #312]
	adrp	x0, .Ls8
	add	x0, x0, :lo12:.Ls8
	str	x0, [x29, #320]
	ldr	x0, [x29, #312]
	ldr	x1, [x29, #320]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.10
.Lmain.11:
	mov	w0, #9
	b	.Lmain.1
.Lmain.10:
	adrp	x0, blanks
	add	x0, x0, :lo12:blanks
	str	x0, [x29, #328]
	ldr	x0, [x29, #328]
	bl	trim
	cmp	w0, #-1
	b.ne	.Lmain.13
	adrp	x0, blanks
	add	x0, x0, :lo12:blanks
	str	x0, [x29, #336]
	adrp	x0, .Ls9
	add	x0, x0, :lo12:.Ls9
	str	x0, [x29, #344]
	ldr	x0, [x29, #336]
	ldr	x1, [x29, #344]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.12
.Lmain.13:
	mov	w0, #10
	b	.Lmain.1
.Lmain.12:
	adrp	x0, x
	add	x0, x0, :lo12:x
	str	x0, [x29, #352]
	ldr	x0, [x29, #352]
	bl	trim
	cmp	w0, #0
	b.ne	.Lmain.15
	adrp	x0, x
	add	x0, x0, :lo12:x
	str	x0, [x29, #360]
	adrp	x0, .Ls10
	add	x0, x0, :lo12:.Ls10
	str	x0, [x29, #368]
	ldr	x0, [x29, #360]
	ldr	x1, [x29, #368]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.14
.Lmain.15:
	mov	w0, #11
	b	.Lmain.1
.Lmain.14:
	adrp	x0, digits
	add	x0, x0, :lo12:digits
	str	x0, [x29, #376]
	ldr	x0, [x29, #376]
	bl	count_digits
	cmp	w0, #6
	b.eq	.Lmain.16
	mov	w0, #12
	b	.Lmain.1
.Lmain.16:
	adrp	x0, .Ls11
	add	x0, x0, :lo12:.Ls11
	str	x0, [x29, #384]
	add	x0, x29, #32
	str	x0, [x29, #392]
	add	x0, x29, #48
	str	x0, [x29, #400]
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #408]
	ldr	x0, [x29, #384]
	ldr	x1, [x29, #392]
	ldr	x2, [x29, #400]
	ldr	x3, [x29, #408]
	bl	printf
	mov	w0, #100
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 416
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #416
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"0"
.Ls1:
	.string	"7"
.Ls2:
	.string	"-7"
.Ls3:
	.string	"1234"
.Ls4:
	.string	"-98765"
.Ls5:
	.string	"2147483647"
.Ls6:
	.string	"cba"
.Ls7:
	.string	""
.Ls8:
	.string	"hello"
.Ls9:
	.string	""
.Ls10:
	.string	"x"
.Ls11:
	.string	"%s %s [%s]\n"
	.data
	.type	word, @object
	.size	word, 9
word:
	.string	"hello \t\n"
	.type	blanks, @object
	.size	blanks, 4
blanks:
	.string	"   "
	.type	x, @object
	.size	x, 2
x:
	.string	"x"
	.type	abc, @object
	.size	abc, 4
abc:
	.string	"abc"
	.type	empty, @object
	.size	empty, 1
empty:
	.string	""
	.type	digits, @object
	.size	digits, 10
digits:
	.string	"a1b22c333"
	.section	.note.GNU-stack,"",@progbits
