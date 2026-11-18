	.file	"header-keywords.c"
	.text
	.type	binsearch, @function
binsearch:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	str	wzr, [x29, #36]
	ldr	w0, [x29, #32]
	sub	w0, w0, #1
	str	w0, [x29, #40]
.Lbinsearch.2:
	ldr	w0, [x29, #36]
	ldr	w1, [x29, #40]
	cmp	w0, w1
	b.gt	.Lbinsearch.3
	ldr	w0, [x29, #36]
	ldr	w1, [x29, #40]
	add	w0, w0, w1
	mov	w1, #2
	sdiv	w0, w0, w1
	str	w0, [x29, #44]
	ldr	x0, [x29, #16]
	str	x0, [x29, #56]
	ldr	x0, [x29, #24]
	ldrsw	x1, [x29, #44]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	x0, [x0]
	str	x0, [x29, #64]
	ldr	x0, [x29, #56]
	ldr	x1, [x29, #64]
	bl	strcmp
	str	w0, [x29, #48]
	ldr	w0, [x29, #48]
	cmp	w0, #0
	b.ge	.Lbinsearch.4
	ldr	w0, [x29, #44]
	sub	w0, w0, #1
	str	w0, [x29, #40]
	b	.Lbinsearch.5
.Lbinsearch.4:
	ldr	w0, [x29, #48]
	cmp	w0, #0
	b.le	.Lbinsearch.6
	ldr	w0, [x29, #44]
	add	w0, w0, #1
	str	w0, [x29, #36]
	b	.Lbinsearch.7
.Lbinsearch.6:
	ldr	w0, [x29, #44]
	b	.Lbinsearch.1
.Lbinsearch.7:
.Lbinsearch.5:
	b	.Lbinsearch.2
.Lbinsearch.3:
	mov	w0, #-1
	b	.Lbinsearch.1
.Lbinsearch.1:
	.cfi_def_cfa wsp, 80
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	binsearch, .-binsearch
	.type	getword, @function
getword:
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
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
.Lgetword.2:
	bl	__ctype_b_loc
	ldr	x0, [x0]
	adrp	x16, next
	ldr	x1, [x16, :lo12:next]
	ldrb	w1, [x1]
	ldrh	w0, [x0, w1, sxtw #1]
	and	w0, w0, #0x2000
	cbz	w0, .Lgetword.3
	adrp	x16, next
	ldr	x0, [x16, :lo12:next]
	add	x0, x0, #1
	adrp	x16, next
	str	x0, [x16, :lo12:next]
	b	.Lgetword.2
.Lgetword.3:
	adrp	x16, next
	ldr	x0, [x16, :lo12:next]
	ldrb	w0, [x0]
	cmp	w0, #0
	b.ne	.Lgetword.4
	mov	w0, #0
	b	.Lgetword.1
.Lgetword.4:
	ldr	x0, [x29, #32]
	add	x1, x0, #1
	str	x1, [x29, #32]
	adrp	x16, next
	ldr	x1, [x16, :lo12:next]
	ldrb	w1, [x1]
	strb	w1, [x0]
	bl	__ctype_b_loc
	ldr	x0, [x0]
	adrp	x16, next
	ldr	x1, [x16, :lo12:next]
	add	x2, x1, #1
	adrp	x16, next
	str	x2, [x16, :lo12:next]
	ldrb	w1, [x1]
	ldrh	w0, [x0, w1, sxtw #1]
	and	w0, w0, #0x400
	cbnz	w0, .Lgetword.5
	ldr	x0, [x29, #32]
	strb	wzr, [x0]
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	b	.Lgetword.1
.Lgetword.5:
.Lgetword.6:
	ldr	w0, [x29, #24]
	sub	w0, w0, #1
	str	w0, [x29, #24]
	cmp	w0, #0
	b.le	.Lgetword.8
	bl	__ctype_b_loc
	ldr	x0, [x0]
	adrp	x16, next
	ldr	x1, [x16, :lo12:next]
	ldrb	w1, [x1]
	ldrh	w0, [x0, w1, sxtw #1]
	and	w0, w0, #0x8
	cbz	w0, .Lgetword.8
	ldr	x0, [x29, #32]
	add	x1, x0, #1
	str	x1, [x29, #32]
	adrp	x16, next
	ldr	x1, [x16, :lo12:next]
	ldrb	w1, [x1]
	strb	w1, [x0]
	adrp	x16, next
	ldr	x0, [x16, :lo12:next]
	add	x0, x0, #1
	adrp	x16, next
	str	x0, [x16, :lo12:next]
	b	.Lgetword.6
.Lgetword.8:
	ldr	x0, [x29, #32]
	strb	wzr, [x0]
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	b	.Lgetword.1
.Lgetword.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	getword, .-getword
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #176
	.cfi_def_cfa_offset 176
	stp	x29, x30, [sp]
	.cfi_offset w29, -176
	.cfi_offset w30, -168
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #0
	cbz	w0, .Lmain.2
	mov	w0, #201
	b	.Lmain.1
.Lmain.2:
	adrp	x16, keytab+504
	ldr	w0, [x16, :lo12:keytab+504]
	cmp	w0, #0
	b.ne	.Lmain.4
	adrp	x16, keytab+496
	ldr	x0, [x16, :lo12:keytab+496]
	str	x0, [x29, #56]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #64]
	ldr	x0, [x29, #56]
	ldr	x1, [x29, #64]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.3
.Lmain.4:
	mov	w0, #202
	b	.Lmain.1
.Lmain.3:
.Lmain.5:
	add	x0, x29, #16
	str	x0, [x29, #72]
	mov	w0, #32
	str	w0, [x29, #80]
	ldr	x0, [x29, #72]
	ldr	w1, [x29, #80]
	bl	getword
	cmp	w0, #0
	b.eq	.Lmain.6
	bl	__ctype_b_loc
	ldr	x0, [x0]
	ldrb	w1, [x29, #16]
	ldrh	w0, [x0, w1, sxtw #1]
	and	w0, w0, #0x400
	cbz	w0, .Lmain.7
	add	x0, x29, #16
	str	x0, [x29, #88]
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	str	x0, [x29, #96]
	mov	w0, #32
	str	w0, [x29, #104]
	ldr	x0, [x29, #88]
	ldr	x1, [x29, #96]
	ldr	w2, [x29, #104]
	bl	binsearch
	str	w0, [x29, #48]
	cmp	w0, #0
	b.lt	.Lmain.8
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	ldrsw	x1, [x29, #48]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w1, [x0, #8]
	add	w1, w1, #1
	str	w1, [x0, #8]
.Lmain.8:
.Lmain.7:
	b	.Lmain.5
.Lmain.6:
	str	wzr, [x29, #108]
	str	wzr, [x29, #112]
	str	wzr, [x29, #48]
.Lmain.9:
	ldr	w0, [x29, #48]
	cmp	w0, #32
	b.ge	.Lmain.11
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	ldrsw	x1, [x29, #48]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	cmp	w0, #0
	b.le	.Lmain.12
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #120]
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	ldrsw	x1, [x29, #48]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	str	w0, [x29, #128]
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	ldrsw	x1, [x29, #48]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	x0, [x0]
	str	x0, [x29, #136]
	ldr	x0, [x29, #120]
	ldr	w1, [x29, #128]
	ldr	x2, [x29, #136]
	bl	printf
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	ldrsw	x1, [x29, #48]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	ldr	w1, [x29, #108]
	add	w1, w1, w0
	str	w1, [x29, #108]
	ldr	w0, [x29, #112]
	add	w0, w0, #1
	str	w0, [x29, #112]
.Lmain.12:
	ldr	w0, [x29, #48]
	add	w0, w0, #1
	str	w0, [x29, #48]
	b	.Lmain.9
.Lmain.11:
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	str	x0, [x29, #144]
	adrp	x0, keytab
	add	x0, x0, :lo12:keytab
	str	x0, [x29, #152]
	mov	w0, #32
	str	w0, [x29, #160]
	ldr	x0, [x29, #144]
	ldr	x1, [x29, #152]
	ldr	w2, [x29, #160]
	bl	binsearch
	cmp	w0, #-1
	b.eq	.Lmain.13
	mov	w0, #203
	b	.Lmain.1
.Lmain.13:
	ldr	w0, [x29, #108]
	ldr	w1, [x29, #112]
	add	w0, w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 176
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #176
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"while"
.Ls1:
	.string	"%4d %s\n"
.Ls2:
	.string	"main"
.Ls3:
	.string	"auto"
.Ls4:
	.string	"break"
.Ls5:
	.string	"case"
.Ls6:
	.string	"char"
.Ls7:
	.string	"const"
.Ls8:
	.string	"continue"
.Ls9:
	.string	"default"
.Ls10:
	.string	"do"
.Ls11:
	.string	"double"
.Ls12:
	.string	"else"
.Ls13:
	.string	"enum"
.Ls14:
	.string	"extern"
.Ls15:
	.string	"float"
.Ls16:
	.string	"for"
.Ls17:
	.string	"goto"
.Ls18:
	.string	"if"
.Ls19:
	.string	"int"
.Ls20:
	.string	"long"
.Ls21:
	.string	"register"
.Ls22:
	.string	"return"
.Ls23:
	.string	"short"
.Ls24:
	.string	"signed"
.Ls25:
	.string	"sizeof"
.Ls26:
	.string	"static"
.Ls27:
	.string	"struct"
.Ls28:
	.string	"switch"
.Ls29:
	.string	"typedef"
.Ls30:
	.string	"union"
.Ls31:
	.string	"unsigned"
.Ls32:
	.string	"void"
.Ls33:
	.string	"volatile"
.Ls34:
	.string	"while"
	.data
	.type	keytab, @object
	.size	keytab, 512
	.p2align 3
keytab:
	.quad	.Ls3
	.long	0
	.zero	4
	.quad	.Ls4
	.long	0
	.zero	4
	.quad	.Ls5
	.long	0
	.zero	4
	.quad	.Ls6
	.long	0
	.zero	4
	.quad	.Ls7
	.long	0
	.zero	4
	.quad	.Ls8
	.long	0
	.zero	4
	.quad	.Ls9
	.long	0
	.zero	4
	.quad	.Ls10
	.long	0
	.zero	4
	.quad	.Ls11
	.long	0
	.zero	4
	.quad	.Ls12
	.long	0
	.zero	4
	.quad	.Ls13
	.long	0
	.zero	4
	.quad	.Ls14
	.long	0
	.zero	4
	.quad	.Ls15
	.long	0
	.zero	4
	.quad	.Ls16
	.long	0
	.zero	4
	.quad	.Ls17
	.long	0
	.zero	4
	.quad	.Ls18
	.long	0
	.zero	4
	.quad	.Ls19
	.long	0
	.zero	4
	.quad	.Ls20
	.long	0
	.zero	4
	.quad	.Ls21
	.long	0
	.zero	4
	.quad	.Ls22
	.long	0
	.zero	4
	.quad	.Ls23
	.long	0
	.zero	4
	.quad	.Ls24
	.long	0
	.zero	4
	.quad	.Ls25
	.long	0
	.zero	4
	.quad	.Ls26
	.long	0
	.zero	4
	.quad	.Ls27
	.long	0
	.zero	4
	.quad	.Ls28
	.long	0
	.zero	4
	.quad	.Ls29
	.long	0
	.zero	4
	.quad	.Ls30
	.long	0
	.zero	4
	.quad	.Ls31
	.long	0
	.zero	4
	.quad	.Ls32
	.long	0
	.zero	4
	.quad	.Ls33
	.long	0
	.zero	4
	.quad	.Ls34
	.zero	8
	.type	text, @object
	.size	text, 275
text:
	.string	"static int count(const char *s)\n{\n        int n = 0;\n        while (*s != '\\0')\n                if (*s++ == ' ')\n                        n++;\n                else\n                        continue;\n        for (;;)\n                break;\n        return (int) sizeof n + n;\n}\n"
	.type	next, @object
	.size	next, 8
	.p2align 3
next:
	.quad	text
	.section	.note.GNU-stack,"",@progbits
