	.file	"header-wide.c"
	.text
	.type	length, @function
length:
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
.Llength.2:
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldr	w0, [x0, w1, sxtw #2]
	cmp	w0, #0
	b.eq	.Llength.3
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Llength.2
.Llength.3:
	ldr	w0, [x29, #24]
	b	.Llength.1
.Llength.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	length, .-length
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
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #16]
	str	wzr, [x29, #24]
	str	wzr, [x29, #28]
.Lmain.2:
	adrp	x0, word
	add	x0, x0, :lo12:word
	ldr	w1, [x29, #28]
	ldr	w0, [x0, w1, sxtw #2]
	cmp	w0, #0
	b.eq	.Lmain.4
	adrp	x0, word
	add	x0, x0, :lo12:word
	ldr	w1, [x29, #28]
	ldr	w0, [x0, w1, sxtw #2]
	cmp	w0, #127
	b.ls	.Lmain.5
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
.Lmain.5:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lmain.2
.Lmain.4:
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #32]
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	length
	str	w0, [x29, #40]
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #64]
	ldr	x0, [x29, #64]
	bl	wcslen
	str	w0, [x29, #56]
	ldr	w0, [x29, #24]
	str	w0, [x29, #72]
	mov	w0, #52
	str	w0, [x29, #76]
	ldr	x0, [x29, #32]
	ldr	w1, [x29, #40]
	ldr	w2, [x29, #56]
	ldr	w3, [x29, #72]
	ldr	w4, [x29, #76]
	bl	printf
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #80]
	ldr	x0, [x29, #80]
	bl	length
	cmp	w0, #12
	b.ne	.Lmain.7
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #88]
	ldr	x0, [x29, #88]
	bl	wcslen
	cmp	x0, #12
	b.ne	.Lmain.7
	ldr	w0, [x29, #24]
	cmp	w0, #3
	b.eq	.Lmain.6
.Lmain.7:
	mov	w0, #201
	b	.Lmain.1
.Lmain.6:
	adrp	x16, word+8
	ldr	w0, [x16, :lo12:word+8]
	cmp	w0, #232
	b.ne	.Lmain.9
	adrp	x16, word+40
	ldr	w0, [x16, :lo12:word+40]
	cmp	w0, #233
	b.ne	.Lmain.9
	mov	w0, #0
	cbz	w0, .Lmain.8
.Lmain.9:
	mov	w0, #202
	b	.Lmain.1
.Lmain.8:
	mov	w0, #0
	cbz	w0, .Lmain.10
	mov	w0, #203
	b	.Lmain.1
.Lmain.10:
	mov	w0, #0
	cbnz	w0, .Lmain.12
	ldr	x0, [x29, #16]
	str	x0, [x29, #96]
	ldr	x0, [x29, #96]
	bl	length
	cmp	w0, #3
	b.ne	.Lmain.12
	ldr	x0, [x29, #16]
	ldr	w0, [x0, #8]
	cmp	w0, #99
	b.eq	.Lmain.11
.Lmain.12:
	mov	w0, #204
	b	.Lmain.1
.Lmain.11:
	mov	w0, #0
	cbnz	w0, .Lmain.14
	adrp	x16, smile+2
	ldrh	w0, [x16, :lo12:smile+2]
	mov	w1, #55357
	cmp	w0, w1
	b.ne	.Lmain.14
	adrp	x16, smile+4
	ldrh	w0, [x16, :lo12:smile+4]
	mov	w1, #56832
	cmp	w0, w1
	b.eq	.Lmain.13
.Lmain.14:
	mov	w0, #205
	b	.Lmain.1
.Lmain.13:
	mov	w0, #0
	cbz	w0, .Lmain.15
	mov	w0, #206
	b	.Lmain.1
.Lmain.15:
	adrp	x0, word
	add	x0, x0, :lo12:word
	str	x0, [x29, #104]
	ldr	x0, [x29, #104]
	bl	length
	mov	w1, #10
	mul	w0, w0, w1
	ldr	w1, [x29, #24]
	add	w0, w0, w1
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
	.p2align 2
.Ls0:
	.long	97, 98, 99, 0
.Ls1:
	.string	"%d %d %d %d\n"
	.data
	.type	word, @object
	.size	word, 52
	.p2align 2
word:
	.long	99, 114, 232, 109, 101, 32, 98, 114, 251, 108, 233, 101, 0
	.type	smile, @object
	.size	smile, 8
	.p2align 1
smile:
	.short	233, 55357, 56832, 0
	.section	.note.GNU-stack,"",@progbits
