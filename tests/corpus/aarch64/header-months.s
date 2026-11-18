	.file	"header-months.c"
	.text
	.globl	month
	.type	month, @function
month:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, months
	add	x0, x0, :lo12:months
	ldr	w1, [x29, #16]
	sub	w1, w1, #1
	mov	w2, #3
	mul	w1, w1, w2
	add	x0, x0, w1, sxtw
	b	.Lmonth.1
.Lmonth.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	month, .-month
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
	adrp	x16, print
	ldr	x0, [x16, :lo12:print]
	str	x0, [x29, #32]
	adrp	x16, title
	ldr	x0, [x16, :lo12:title]
	str	x0, [x29, #40]
	adrp	x16, out
	ldr	x0, [x16, :lo12:out]
	ldr	x0, [x0]
	str	x0, [x29, #48]
	ldr	x0, [x29, #40]
	ldr	x1, [x29, #48]
	ldr	x16, [x29, #32]
	blr	x16
	cmp	w0, #0
	b.ge	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	mov	w0, #1
	str	w0, [x29, #16]
.Lmain.3:
	ldr	w0, [x29, #16]
	cmp	w0, #12
	b.gt	.Lmain.5
	adrp	x16, put
	ldr	x0, [x16, :lo12:put]
	str	x0, [x29, #56]
	mov	w0, #32
	str	w0, [x29, #64]
	ldr	w0, [x29, #64]
	ldr	x16, [x29, #56]
	blr	x16
	ldr	w0, [x29, #16]
	str	w0, [x29, #68]
	ldr	w0, [x29, #68]
	bl	month
	str	x0, [x29, #24]
	str	wzr, [x29, #20]
.Lmain.6:
	ldr	w0, [x29, #20]
	cmp	w0, #3
	b.ge	.Lmain.8
	adrp	x16, put
	ldr	x0, [x16, :lo12:put]
	str	x0, [x29, #72]
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #20]
	ldrb	w0, [x0, w1, sxtw]
	str	w0, [x29, #80]
	ldr	w0, [x29, #80]
	ldr	x16, [x29, #72]
	blr	x16
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lmain.6
.Lmain.8:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lmain.3
.Lmain.5:
	adrp	x16, print
	ldr	x0, [x16, :lo12:print]
	str	x0, [x29, #88]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #96]
	adrp	x16, out
	ldr	x0, [x16, :lo12:out]
	ldr	x0, [x0]
	str	x0, [x29, #104]
	ldr	x0, [x29, #96]
	ldr	x1, [x29, #104]
	ldr	x16, [x29, #88]
	blr	x16
	cmp	w0, #0
	b.ge	.Lmain.9
	mov	w0, #2
	b	.Lmain.1
.Lmain.9:
	adrp	x16, out
	ldr	x0, [x16, :lo12:out]
	ldr	x0, [x0]
	adrp	x1, :got:stdout
	ldr	x1, [x1, :got_lo12:stdout]
	ldr	x1, [x1]
	cmp	x0, x1
	b.eq	.Lmain.10
	mov	w0, #3
	b	.Lmain.1
.Lmain.10:
	adrp	x16, put
	ldr	x0, [x16, :lo12:put]
	adrp	x1, :got:putchar
	ldr	x1, [x1, :got_lo12:putchar]
	cmp	x0, x1
	b.eq	.Lmain.11
	mov	w0, #4
	b	.Lmain.1
.Lmain.11:
	mov	w0, #37
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
	.string	"\n"
.Ls1:
	.string	"months:"
	.data
	.globl	months
	.type	months, @object
	.size	months, 37
months:
	.string	"JanFebMarAprMayJunJulAugSepOctNovDec"
	.globl	title
	.type	title, @object
	.size	title, 8
	.p2align 3
title:
	.quad	.Ls1
	.globl	out
	.type	out, @object
	.size	out, 8
	.p2align 3
out:
	.quad	stdout
	.globl	put
	.type	put, @object
	.size	put, 8
	.p2align 3
put:
	.quad	putchar
	.globl	print
	.type	print, @object
	.size	print, 8
	.p2align 3
print:
	.quad	fputs
	.section	.note.GNU-stack,"",@progbits
