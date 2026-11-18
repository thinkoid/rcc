	.file	"header-words.c"
	.text
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
	adrp	x0, text
	add	x0, x0, :lo12:text
	adrp	x16, at
	ldr	w1, [x16, :lo12:at]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #0
	b.eq	.Lgetword.3
	bl	__ctype_b_loc
	ldr	x0, [x0]
	adrp	x1, text
	add	x1, x1, :lo12:text
	adrp	x16, at
	ldr	w2, [x16, :lo12:at]
	ldrb	w1, [x1, w2, sxtw]
	ldrh	w0, [x0, w1, sxtw #1]
	and	w0, w0, #0x400
	cbnz	w0, .Lgetword.3
	adrp	x16, at
	ldr	w0, [x16, :lo12:at]
	add	w0, w0, #1
	adrp	x16, at
	str	w0, [x16, :lo12:at]
	b	.Lgetword.2
.Lgetword.3:
	adrp	x0, text
	add	x0, x0, :lo12:text
	adrp	x16, at
	ldr	w1, [x16, :lo12:at]
	ldrb	w0, [x0, w1, sxtw]
	cmp	w0, #0
	b.ne	.Lgetword.4
	mov	w0, #0
	b	.Lgetword.1
.Lgetword.4:
.Lgetword.5:
	bl	__ctype_b_loc
	ldr	x0, [x0]
	adrp	x1, text
	add	x1, x1, :lo12:text
	adrp	x16, at
	ldr	w2, [x16, :lo12:at]
	ldrb	w1, [x1, w2, sxtw]
	ldrh	w0, [x0, w1, sxtw #1]
	and	w0, w0, #0x400
	cbz	w0, .Lgetword.6
	ldr	w0, [x29, #24]
	sub	w0, w0, #1
	str	w0, [x29, #24]
	cmp	w0, #0
	b.le	.Lgetword.6
	ldr	x0, [x29, #32]
	add	x1, x0, #1
	str	x1, [x29, #32]
	adrp	x1, text
	add	x1, x1, :lo12:text
	adrp	x16, at
	ldr	w2, [x16, :lo12:at]
	add	w3, w2, #1
	adrp	x16, at
	str	w3, [x16, :lo12:at]
	ldrb	w1, [x1, w2, sxtw]
	strb	w1, [x0]
	b	.Lgetword.5
.Lgetword.6:
	ldr	x0, [x29, #32]
	strb	wzr, [x0]
	mov	w0, #1
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
	.type	addtree, @function
addtree:
	.cfi_startproc
	sub	sp, sp, #128
	.cfi_def_cfa_offset 128
	stp	x29, x30, [sp]
	.cfi_offset w29, -128
	.cfi_offset w30, -120
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	cmp	x0, #0
	b.ne	.Laddtree.2
	mov	x0, #32
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	malloc
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #48]
	ldr	x0, [x29, #24]
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	bl	strdup
	ldr	x1, [x29, #48]
	str	x0, [x1]
	ldr	x0, [x29, #16]
	mov	w1, #1
	str	w1, [x0, #8]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #16]
	mov	x2, #0
	str	x2, [x1, #24]
	str	x2, [x0, #16]
	b	.Laddtree.3
.Laddtree.2:
	ldr	x0, [x29, #24]
	str	x0, [x29, #64]
	ldr	x0, [x29, #16]
	ldr	x0, [x0]
	str	x0, [x29, #72]
	ldr	x0, [x29, #64]
	ldr	x1, [x29, #72]
	bl	strcmp
	str	w0, [x29, #32]
	cmp	w0, #0
	b.ne	.Laddtree.4
	ldr	x0, [x29, #16]
	ldr	w1, [x0, #8]
	add	w1, w1, #1
	str	w1, [x0, #8]
	b	.Laddtree.5
.Laddtree.4:
	ldr	w0, [x29, #32]
	cmp	w0, #0
	b.ge	.Laddtree.6
	ldr	x0, [x29, #16]
	str	x0, [x29, #80]
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #16]
	str	x0, [x29, #88]
	ldr	x0, [x29, #24]
	str	x0, [x29, #96]
	ldr	x0, [x29, #88]
	ldr	x1, [x29, #96]
	bl	addtree
	ldr	x1, [x29, #80]
	str	x0, [x1, #16]
	b	.Laddtree.7
.Laddtree.6:
	ldr	x0, [x29, #16]
	str	x0, [x29, #104]
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #24]
	str	x0, [x29, #112]
	ldr	x0, [x29, #24]
	str	x0, [x29, #120]
	ldr	x0, [x29, #112]
	ldr	x1, [x29, #120]
	bl	addtree
	ldr	x1, [x29, #104]
	str	x0, [x1, #24]
.Laddtree.7:
.Laddtree.5:
.Laddtree.3:
	ldr	x0, [x29, #16]
	b	.Laddtree.1
.Laddtree.1:
	.cfi_def_cfa wsp, 128
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #128
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	addtree, .-addtree
	.type	treeprint, @function
treeprint:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	cmp	x0, #0
	b.ne	.Ltreeprint.2
	mov	w0, #0
	b	.Ltreeprint.1
.Ltreeprint.2:
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	treeprint
	str	w0, [x29, #24]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #40]
	ldr	x0, [x29, #16]
	ldr	w0, [x0, #8]
	str	w0, [x29, #48]
	ldr	x0, [x29, #16]
	ldr	x0, [x0]
	str	x0, [x29, #56]
	ldr	x0, [x29, #40]
	ldr	w1, [x29, #48]
	ldr	x2, [x29, #56]
	bl	printf
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	x0, [x29, #64]
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #24]
	str	x0, [x29, #72]
	ldr	x0, [x29, #72]
	bl	treeprint
	ldr	x1, [x29, #64]
	add	w0, w1, w0
	b	.Ltreeprint.1
.Ltreeprint.1:
	.cfi_def_cfa wsp, 80
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	treeprint, .-treeprint
	.type	count, @function
count:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
.Lcount.2:
	ldr	x0, [x29, #16]
	cmp	x0, #0
	b.eq	.Lcount.3
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #16]
	ldr	x0, [x0]
	str	x0, [x29, #48]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #48]
	bl	strcmp
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	cmp	w0, #0
	b.ne	.Lcount.4
	ldr	x0, [x29, #16]
	ldr	w0, [x0, #8]
	b	.Lcount.1
.Lcount.4:
	ldr	w0, [x29, #32]
	cmp	w0, #0
	b.ge	.Lcount.5
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #16]
	b	.Lcount.6
.Lcount.5:
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #24]
.Lcount.6:
	str	x0, [x29, #16]
	b	.Lcount.2
.Lcount.3:
	mov	w0, #0
	b	.Lcount.1
.Lcount.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	count, .-count
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #272
	.cfi_def_cfa_offset 272
	stp	x29, x30, [sp]
	.cfi_offset w29, -272
	.cfi_offset w30, -264
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	xzr, [x29, #16]
	str	wzr, [x29, #124]
.Lmain.2:
	add	x0, x29, #24
	str	x0, [x29, #128]
	mov	w0, #100
	str	w0, [x29, #136]
	ldr	x0, [x29, #128]
	ldr	w1, [x29, #136]
	bl	getword
	cbz	w0, .Lmain.3
	ldr	x0, [x29, #16]
	str	x0, [x29, #144]
	add	x0, x29, #24
	str	x0, [x29, #152]
	ldr	x0, [x29, #144]
	ldr	x1, [x29, #152]
	bl	addtree
	str	x0, [x29, #16]
	ldr	w0, [x29, #124]
	add	w0, w0, #1
	str	w0, [x29, #124]
	b	.Lmain.2
.Lmain.3:
	ldr	x0, [x29, #16]
	str	x0, [x29, #168]
	ldr	x0, [x29, #168]
	bl	treeprint
	str	w0, [x29, #160]
	ldr	w0, [x29, #124]
	cmp	w0, #20
	b.eq	.Lmain.4
	mov	w0, #101
	b	.Lmain.1
.Lmain.4:
	ldr	x0, [x29, #16]
	str	x0, [x29, #176]
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #184]
	ldr	x0, [x29, #176]
	ldr	x1, [x29, #184]
	bl	count
	cmp	w0, #2
	b.ne	.Lmain.6
	ldr	x0, [x29, #16]
	str	x0, [x29, #192]
	adrp	x0, .Ls2
	add	x0, x0, :lo12:.Ls2
	str	x0, [x29, #200]
	ldr	x0, [x29, #192]
	ldr	x1, [x29, #200]
	bl	count
	cmp	w0, #2
	b.eq	.Lmain.5
.Lmain.6:
	mov	w0, #102
	b	.Lmain.1
.Lmain.5:
	ldr	x0, [x29, #16]
	str	x0, [x29, #208]
	adrp	x0, .Ls3
	add	x0, x0, :lo12:.Ls3
	str	x0, [x29, #216]
	ldr	x0, [x29, #208]
	ldr	x1, [x29, #216]
	bl	count
	cmp	w0, #2
	b.ne	.Lmain.8
	ldr	x0, [x29, #16]
	str	x0, [x29, #224]
	adrp	x0, .Ls4
	add	x0, x0, :lo12:.Ls4
	str	x0, [x29, #232]
	ldr	x0, [x29, #224]
	ldr	x1, [x29, #232]
	bl	count
	cmp	w0, #1
	b.eq	.Lmain.7
.Lmain.8:
	mov	w0, #103
	b	.Lmain.1
.Lmain.7:
	ldr	w0, [x29, #160]
	mov	w1, #3
	mul	w0, w0, w1
	str	x0, [x29, #240]
	ldr	x0, [x29, #16]
	str	x0, [x29, #248]
	adrp	x0, .Ls5
	add	x0, x0, :lo12:.Ls5
	str	x0, [x29, #256]
	ldr	x0, [x29, #248]
	ldr	x1, [x29, #256]
	bl	count
	ldr	x1, [x29, #240]
	add	w0, w1, w0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 272
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #272
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"%4d %s\n"
.Ls1:
	.string	"of"
.Ls2:
	.string	"tree"
.Ls3:
	.string	"a"
.Ls4:
	.string	"count"
.Ls5:
	.string	"the"
	.data
	.type	text, @object
	.size	text, 90
text:
	.string	"the shape of a tree: the root, the nodes of the tree, a word and its count, left or right"
	.bss
	.type	at, @object
	.size	at, 4
	.p2align 2
at:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
