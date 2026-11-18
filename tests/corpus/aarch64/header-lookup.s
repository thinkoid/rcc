	.file	"header-lookup.c"
	.text
	.type	hash, @function
hash:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	wzr, [x29, #24]
.Lhash.2:
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	cmp	w0, #0
	b.eq	.Lhash.4
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	ldr	w1, [x29, #24]
	mov	w2, #31
	mul	w1, w1, w2
	add	w0, w0, w1
	str	w0, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #1
	str	x0, [x29, #16]
	b	.Lhash.2
.Lhash.4:
	ldr	w0, [x29, #24]
	mov	w1, #101
	udiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lhash.1
.Lhash.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	hash, .-hash
	.type	lookup, @function
lookup:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x0, hashtab
	add	x0, x0, :lo12:hashtab
	str	x0, [x29, #32]
	ldr	x0, [x29, #16]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	hash
	ldr	x1, [x29, #32]
	ldr	x0, [x1, w0, uxtw #3]
	str	x0, [x29, #24]
.Llookup.2:
	ldr	x0, [x29, #24]
	cmp	x0, #0
	b.eq	.Llookup.4
	ldr	x0, [x29, #16]
	str	x0, [x29, #48]
	ldr	x0, [x29, #24]
	ldr	x0, [x0, #8]
	str	x0, [x29, #56]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #56]
	bl	strcmp
	cmp	w0, #0
	b.ne	.Llookup.5
	ldr	x0, [x29, #24]
	b	.Llookup.1
.Llookup.5:
	ldr	x0, [x29, #24]
	ldr	x0, [x0]
	str	x0, [x29, #24]
	b	.Llookup.2
.Llookup.4:
	mov	x0, #0
	b	.Llookup.1
.Llookup.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	lookup, .-lookup
	.type	install, @function
install:
	.cfi_startproc
	sub	sp, sp, #112
	.cfi_def_cfa_offset 112
	stp	x29, x30, [sp]
	.cfi_offset w29, -112
	.cfi_offset w30, -104
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	lookup
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	cmp	x0, #0
	b.ne	.Linstall.2
	mov	x0, #24
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	malloc
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	cmp	x0, #0
	b.eq	.Linstall.4
	ldr	x0, [x29, #32]
	str	x0, [x29, #56]
	ldr	x0, [x29, #16]
	str	x0, [x29, #64]
	ldr	x0, [x29, #64]
	bl	strdup
	ldr	x1, [x29, #56]
	str	x0, [x1, #8]
	cmp	x0, #0
	b.ne	.Linstall.3
.Linstall.4:
	mov	x0, #0
	b	.Linstall.1
.Linstall.3:
	ldr	x0, [x29, #16]
	str	x0, [x29, #80]
	ldr	x0, [x29, #80]
	bl	hash
	str	w0, [x29, #72]
	ldr	x0, [x29, #32]
	adrp	x1, hashtab
	add	x1, x1, :lo12:hashtab
	ldr	w2, [x29, #72]
	ldr	x1, [x1, x2, lsl #3]
	str	x1, [x0]
	adrp	x0, hashtab
	add	x0, x0, :lo12:hashtab
	ldr	w1, [x29, #72]
	ldr	x2, [x29, #32]
	str	x2, [x0, x1, lsl #3]
	b	.Linstall.5
.Linstall.2:
	ldr	x0, [x29, #32]
	ldr	x0, [x0, #16]
	str	x0, [x29, #88]
	ldr	x0, [x29, #88]
	bl	free
.Linstall.5:
	ldr	x0, [x29, #32]
	str	x0, [x29, #96]
	ldr	x0, [x29, #24]
	str	x0, [x29, #104]
	ldr	x0, [x29, #104]
	bl	strdup
	ldr	x1, [x29, #96]
	str	x0, [x1, #16]
	cmp	x0, #0
	b.ne	.Linstall.6
	mov	x0, #0
	b	.Linstall.1
.Linstall.6:
	ldr	x0, [x29, #32]
	b	.Linstall.1
.Linstall.1:
	.cfi_def_cfa wsp, 112
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #112
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	install, .-install
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #240
	.cfi_def_cfa_offset 240
	stp	x29, x30, [sp]
	.cfi_offset w29, -240
	.cfi_offset w30, -232
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #16]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	bl	install
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	str	x0, [x29, #32]
	adrp	x0, .Ls3
	add	x0, x0, :lo12:.Ls3
	str	x0, [x29, #40]
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #40]
	bl	install
	adrp	x0, .Ls4
	add	x0, x0, :lo12:.Ls4
	str	x0, [x29, #48]
	adrp	x0, .Ls5
	add	x0, x0, :lo12:.Ls5
	str	x0, [x29, #56]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #56]
	bl	install
	adrp	x0, .Ls6
	add	x0, x0, :lo12:.Ls6
	str	x0, [x29, #64]
	adrp	x0, .Ls7
	add	x0, x0, :lo12:.Ls7
	str	x0, [x29, #72]
	ldr	x0, [x29, #64]
	ldr	x1, [x29, #72]
	bl	install
	adrp	x0, .Ls8
	add	x0, x0, :lo12:.Ls8
	str	x0, [x29, #80]
	adrp	x0, .Ls9
	add	x0, x0, :lo12:.Ls9
	str	x0, [x29, #88]
	ldr	x0, [x29, #80]
	ldr	x1, [x29, #88]
	bl	install
	adrp	x0, .Ls10
	add	x0, x0, :lo12:.Ls10
	str	x0, [x29, #96]
	adrp	x0, .Ls11
	add	x0, x0, :lo12:.Ls11
	str	x0, [x29, #104]
	ldr	x0, [x29, #96]
	ldr	x1, [x29, #104]
	bl	install
	adrp	x0, .Ls12
	add	x0, x0, :lo12:.Ls12
	str	x0, [x29, #112]
	adrp	x0, .Ls13
	add	x0, x0, :lo12:.Ls13
	str	x0, [x29, #120]
	ldr	x0, [x29, #112]
	ldr	x1, [x29, #120]
	bl	install
	adrp	x0, .Ls14
	add	x0, x0, :lo12:.Ls14
	str	x0, [x29, #128]
	adrp	x0, .Ls15
	add	x0, x0, :lo12:.Ls15
	str	x0, [x29, #136]
	ldr	x0, [x29, #128]
	ldr	x1, [x29, #136]
	bl	install
	str	wzr, [x29, #144]
	str	wzr, [x29, #148]
.Lmain.2:
	ldr	w0, [x29, #148]
	cmp	w0, #7
	b.ge	.Lmain.4
	adrp	x0, names
	add	x0, x0, :lo12:names
	ldr	w1, [x29, #148]
	ldr	x0, [x0, w1, sxtw #3]
	str	x0, [x29, #160]
	ldr	x0, [x29, #160]
	bl	lookup
	str	x0, [x29, #152]
	ldr	x0, [x29, #152]
	cmp	x0, #0
	b.ne	.Lmain.5
	mov	w0, #101
	b	.Lmain.1
.Lmain.5:
	adrp	x0, .Ls16
	add	x0, x0, :lo12:.Ls16
	str	x0, [x29, #168]
	ldr	x0, [x29, #152]
	ldr	x0, [x0, #8]
	str	x0, [x29, #176]
	ldr	x0, [x29, #152]
	ldr	x0, [x0, #16]
	str	x0, [x29, #184]
	ldr	x0, [x29, #168]
	ldr	x1, [x29, #176]
	ldr	x2, [x29, #184]
	bl	printf
	ldr	x0, [x29, #152]
	ldr	x0, [x0, #16]
	str	x0, [x29, #192]
	ldr	x0, [x29, #192]
	bl	strlen
	ldr	w1, [x29, #144]
	add	w1, w1, w0
	str	w1, [x29, #144]
	ldr	w0, [x29, #148]
	add	w0, w0, #1
	str	w0, [x29, #148]
	b	.Lmain.2
.Lmain.4:
	adrp	x0, .Ls17
	add	x0, x0, :lo12:.Ls17
	str	x0, [x29, #200]
	ldr	x0, [x29, #200]
	bl	lookup
	cmp	x0, #0
	b.eq	.Lmain.6
	mov	w0, #102
	b	.Lmain.1
.Lmain.6:
	adrp	x0, .Ls18
	add	x0, x0, :lo12:.Ls18
	str	x0, [x29, #216]
	ldr	x0, [x29, #216]
	bl	lookup
	ldr	x0, [x0, #16]
	str	x0, [x29, #208]
	adrp	x0, .Ls19
	add	x0, x0, :lo12:.Ls19
	str	x0, [x29, #224]
	ldr	x0, [x29, #208]
	ldr	x1, [x29, #224]
	bl	strcmp
	cmp	w0, #0
	b.eq	.Lmain.7
	mov	w0, #103
	b	.Lmain.1
.Lmain.7:
	str	wzr, [x29, #232]
	str	wzr, [x29, #236]
.Lmain.8:
	ldr	w0, [x29, #236]
	cmp	w0, #101
	b.ge	.Lmain.10
	adrp	x0, hashtab
	add	x0, x0, :lo12:hashtab
	ldr	w1, [x29, #236]
	ldr	x0, [x0, w1, sxtw #3]
	cmp	x0, #0
	b.eq	.Lmain.11
	adrp	x0, hashtab
	add	x0, x0, :lo12:hashtab
	ldr	w1, [x29, #236]
	ldr	x0, [x0, w1, sxtw #3]
	ldr	x0, [x0]
	cmp	x0, #0
	b.eq	.Lmain.11
	ldr	w0, [x29, #232]
	add	w0, w0, #1
	str	w0, [x29, #232]
.Lmain.11:
	ldr	w0, [x29, #236]
	add	w0, w0, #1
	str	w0, [x29, #236]
	b	.Lmain.8
.Lmain.10:
	ldr	w0, [x29, #144]
	ldr	w1, [x29, #232]
	mov	w2, #10
	mul	w1, w1, w2
	add	w0, w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 240
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #240
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"IN"
.Ls1:
	.string	"1"
.Ls2:
	.string	"OUT"
.Ls3:
	.string	"0"
.Ls4:
	.string	"MAX"
.Ls5:
	.string	"100"
.Ls6:
	.string	"EOF"
.Ls7:
	.string	"(-1)"
.Ls8:
	.string	"TAB"
.Ls9:
	.string	"\\t"
.Ls10:
	.string	"YES"
.Ls11:
	.string	"always"
.Ls12:
	.string	"NO"
.Ls13:
	.string	"nil"
.Ls14:
	.string	"IN"
.Ls15:
	.string	"one"
.Ls16:
	.string	"%s %s\n"
.Ls17:
	.string	"OTHER"
.Ls18:
	.string	"IN"
.Ls19:
	.string	"one"
.Ls20:
	.string	"IN"
.Ls21:
	.string	"OUT"
.Ls22:
	.string	"MAX"
.Ls23:
	.string	"EOF"
.Ls24:
	.string	"TAB"
.Ls25:
	.string	"YES"
.Ls26:
	.string	"NO"
	.data
	.type	names, @object
	.size	names, 56
	.p2align 3
names:
	.quad	.Ls20
	.quad	.Ls21
	.quad	.Ls22
	.quad	.Ls23
	.quad	.Ls24
	.quad	.Ls25
	.quad	.Ls26
	.bss
	.type	hashtab, @object
	.size	hashtab, 808
	.p2align 3
hashtab:
	.zero	808
	.section	.note.GNU-stack,"",@progbits
