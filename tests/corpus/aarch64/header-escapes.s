	.file	"header-escapes.c"
	.text
	.type	escape_name, @function
escape_name:
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
	sub	w0, w0, #7
	cmp	w0, #6
	b.hi	.Lescape_name.9
	adrp	x16, .Lescape_name.10
	add	x16, x16, :lo12:.Lescape_name.10
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Lescape_name.10:
	.quad	.Lescape_name.2
	.quad	.Lescape_name.3
	.quad	.Lescape_name.4
	.quad	.Lescape_name.5
	.quad	.Lescape_name.6
	.quad	.Lescape_name.7
	.quad	.Lescape_name.8
	.text
.Lescape_name.2:
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	b	.Lescape_name.1
.Lescape_name.3:
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	b	.Lescape_name.1
.Lescape_name.4:
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	b	.Lescape_name.1
.Lescape_name.5:
	adrp	x0, .Ls3
	add	x0, x0, :lo12:.Ls3
	b	.Lescape_name.1
.Lescape_name.6:
	adrp	x0, .Ls4
	add	x0, x0, :lo12:.Ls4
	b	.Lescape_name.1
.Lescape_name.7:
	adrp	x0, .Ls5
	add	x0, x0, :lo12:.Ls5
	b	.Lescape_name.1
.Lescape_name.8:
	adrp	x0, .Ls6
	add	x0, x0, :lo12:.Ls6
	b	.Lescape_name.1
.Lescape_name.9:
	mov	x0, #0
	b	.Lescape_name.1
.Lescape_name.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	escape_name, .-escape_name
	.type	leap, @function
leap:
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
	mov	w1, #4
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	b.ne	.Lleap.4
	ldr	w0, [x29, #16]
	mov	w1, #100
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	b.ne	.Lleap.3
.Lleap.4:
	ldr	w0, [x29, #16]
	mov	w1, #400
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	b.ne	.Lleap.2
.Lleap.3:
	mov	w0, #1
	b	.Lleap.1
.Lleap.2:
	mov	w0, #0
	b	.Lleap.1
.Lleap.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	leap, .-leap
	.type	days, @function
days:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	adrp	x0, month_days
	ldr	w1, [x29, #16]
	add	x0, x0, x1, lsl #2
	ldr	w0, [x0, :lo12:month_days]
	str	x0, [x29, #24]
	ldr	w0, [x29, #16]
	cmp	w0, #2
	b.ne	.Ldays.2
	ldr	w0, [x29, #20]
	cmp	w0, #1
	b.ne	.Ldays.2
	mov	w0, #1
	b	.Ldays.3
.Ldays.2:
	mov	w0, #0
.Ldays.3:
	ldr	x1, [x29, #24]
	add	w0, w1, w0
	b	.Ldays.1
.Ldays.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	days, .-days
	.type	year_days, @function
year_days:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #24]
	mov	w0, #1
	str	w0, [x29, #20]
.Lyear_days.2:
	ldr	w0, [x29, #20]
	cmp	w0, #12
	b.hi	.Lyear_days.4
	ldr	w0, [x29, #20]
	str	w0, [x29, #28]
	ldr	w0, [x29, #16]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	leap
	str	w0, [x29, #32]
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #32]
	bl	days
	ldr	w1, [x29, #24]
	add	w1, w1, w0
	str	w1, [x29, #24]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lyear_days.2
.Lyear_days.4:
	ldr	w0, [x29, #24]
	b	.Lyear_days.1
.Lyear_days.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	year_days, .-year_days
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #192
	.cfi_def_cfa_offset 192
	stp	x29, x30, [sp]
	.cfi_offset w29, -192
	.cfi_offset w30, -184
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #-1
	str	w0, [x29, #16]
	str	wzr, [x29, #32]
	str	wzr, [x29, #36]
.Lmain.2:
	adrp	x0, text
	ldr	w1, [x29, #36]
	add	x0, x0, w1, sxtw
	ldrb	w0, [x0, :lo12:text]
	cmp	w0, #0
	b.eq	.Lmain.4
	adrp	x0, text
	ldr	w1, [x29, #36]
	add	x0, x0, w1, sxtw
	ldrb	w0, [x0, :lo12:text]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	escape_name
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	cmp	x0, #0
	b.eq	.Lmain.3
	ldr	w0, [x29, #32]
	add	w1, w0, #1
	str	w1, [x29, #32]
	cmp	w0, #0
	b.le	.Lmain.5
	adrp	x0, names
	add	x0, x0, :lo12:names
	str	x0, [x29, #48]
	adrp	x0, .Ls7
	add	x0, x0, :lo12:.Ls7
	str	x0, [x29, #56]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #56]
	bl	strcat
.Lmain.5:
	adrp	x0, names
	add	x0, x0, :lo12:names
	str	x0, [x29, #64]
	ldr	x0, [x29, #24]
	str	x0, [x29, #72]
	ldr	x0, [x29, #64]
	ldr	x1, [x29, #72]
	bl	strcat
.Lmain.3:
	ldr	w0, [x29, #36]
	add	w0, w0, #1
	str	w0, [x29, #36]
	b	.Lmain.2
.Lmain.4:
	adrp	x0, .Ls8
	add	x0, x0, :lo12:.Ls8
	str	x0, [x29, #80]
	adrp	x0, names
	add	x0, x0, :lo12:names
	str	x0, [x29, #88]
	mov	w0, #1988
	str	w0, [x29, #100]
	ldr	w0, [x29, #100]
	bl	year_days
	str	w0, [x29, #96]
	mov	w0, #1900
	str	w0, [x29, #108]
	ldr	w0, [x29, #108]
	bl	year_days
	str	w0, [x29, #104]
	ldr	x0, [x29, #80]
	ldr	x1, [x29, #88]
	ldr	w2, [x29, #96]
	ldr	w3, [x29, #104]
	bl	printf
	adrp	x0, names
	add	x0, x0, :lo12:names
	str	x0, [x29, #112]
	adrp	x0, .Ls9
	add	x0, x0, :lo12:.Ls9
	str	x0, [x29, #120]
	ldr	x0, [x29, #112]
	ldr	x1, [x29, #120]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.6
	mov	w0, #101
	b	.Lmain.1
.Lmain.6:
	mov	w0, #1988
	str	w0, [x29, #128]
	ldr	w0, [x29, #128]
	bl	year_days
	cmp	w0, #366
	b.ne	.Lmain.8
	mov	w0, #1900
	str	w0, [x29, #132]
	ldr	w0, [x29, #132]
	bl	year_days
	cmp	w0, #365
	b.ne	.Lmain.8
	mov	w0, #2000
	str	w0, [x29, #136]
	ldr	w0, [x29, #136]
	bl	year_days
	cmp	w0, #366
	b.eq	.Lmain.7
.Lmain.8:
	mov	w0, #102
	b	.Lmain.1
.Lmain.7:
	mov	w0, #2
	str	w0, [x29, #140]
	mov	w0, #1988
	str	w0, [x29, #148]
	ldr	w0, [x29, #148]
	bl	leap
	str	w0, [x29, #144]
	ldr	w0, [x29, #140]
	ldr	w1, [x29, #144]
	bl	days
	cmp	w0, #29
	b.ne	.Lmain.10
	mov	w0, #2
	str	w0, [x29, #152]
	str	wzr, [x29, #156]
	ldr	w0, [x29, #152]
	ldr	w1, [x29, #156]
	bl	days
	cmp	w0, #28
	b.ne	.Lmain.10
	mov	w0, #12
	str	w0, [x29, #160]
	mov	w0, #1
	str	w0, [x29, #164]
	ldr	w0, [x29, #160]
	ldr	w1, [x29, #164]
	bl	days
	cmp	w0, #31
	b.eq	.Lmain.9
.Lmain.10:
	mov	w0, #103
	b	.Lmain.1
.Lmain.9:
	ldr	w0, [x29, #16]
	cmp	w0, #0
	b.lo	.Lmain.12
	mov	w0, #0
	cbz	w0, .Lmain.11
.Lmain.12:
	mov	w0, #104
	b	.Lmain.1
.Lmain.11:
	mov	w0, #2
	str	w0, [x29, #168]
	mov	w0, #1988
	str	w0, [x29, #176]
	ldr	w0, [x29, #176]
	bl	leap
	str	w0, [x29, #172]
	ldr	w0, [x29, #168]
	ldr	w1, [x29, #172]
	bl	days
	ldr	w1, [x29, #32]
	add	w0, w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 192
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #192
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"BELL"
.Ls1:
	.string	"BACKSPACE"
.Ls2:
	.string	"TAB"
.Ls3:
	.string	"NEWLINE"
.Ls4:
	.string	"VTAB"
.Ls5:
	.string	"FORMFEED"
.Ls6:
	.string	"RETURN"
.Ls7:
	.string	" "
.Ls8:
	.string	"%s; 1988 has %d days, 1900 has %d\n"
.Ls9:
	.string	"TAB NEWLINE BELL RETURN VTAB BACKSPACE FORMFEED"
	.data
	.type	month_days, @object
	.size	month_days, 52
	.p2align 2
month_days:
	.long	0
	.long	31
	.long	28
	.long	31
	.long	30
	.long	31
	.long	30
	.long	31
	.long	31
	.long	30
	.long	31
	.long	30
	.long	31
	.type	text, @object
	.size	text, 15
text:
	.string	"a\tb\nc\007d\re\013f\010g\014"
	.bss
	.type	names, @object
	.size	names, 64
names:
	.zero	64
	.section	.note.GNU-stack,"",@progbits
