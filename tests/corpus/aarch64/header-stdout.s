	.file	"header-stdout.c"
	.text
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
	str	wzr, [x29, #16]
.Lmain.2:
	ldr	w0, [x29, #16]
	cmp	w0, #10
	b.ge	.Lmain.4
	adrp	x0, table
	add	x0, x0, :lo12:table
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #16]
	ldr	w3, [x29, #16]
	mul	w2, w2, w3
	str	w2, [x0, w1, sxtw #2]
	adrp	x16, count
	ldr	w0, [x16, :lo12:count]
	add	w0, w0, #1
	adrp	x16, count
	str	w0, [x16, :lo12:count]
.Lmain.3:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lmain.2
.Lmain.4:
	str	wzr, [x29, #16]
.Lmain.5:
	ldr	w0, [x29, #16]
	adrp	x16, count
	ldr	w1, [x16, :lo12:count]
	cmp	w0, w1
	b.ge	.Lmain.7
	adrp	x0, table
	add	x0, x0, :lo12:table
	ldr	w1, [x29, #16]
	ldr	w0, [x0, w1, sxtw #2]
	sxtw	x0, w0
	adrp	x16, total
	ldr	x1, [x16, :lo12:total]
	add	x1, x1, x0
	adrp	x16, total
	str	x1, [x16, :lo12:total]
.Lmain.6:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lmain.5
.Lmain.7:
	adrp	x16, total
	ldr	x0, [x16, :lo12:total]
	cmp	x0, #285
	b.eq	.Lmain.8
	mov	w0, #1
	b	.Lmain.1
.Lmain.8:
	adrp	x0, :got:stdout
	ldr	x0, [x0, :got_lo12:stdout]
	ldr	x0, [x0]
	str	x0, [x29, #24]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #32]
	adrp	x16, count
	ldr	w0, [x16, :lo12:count]
	str	w0, [x29, #40]
	adrp	x16, total
	ldr	x0, [x16, :lo12:total]
	str	x0, [x29, #48]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #32]
	ldr	w2, [x29, #40]
	ldr	x3, [x29, #48]
	bl	fprintf
	cmp	w0, #22
	b.eq	.Lmain.9
	mov	w0, #2
	b	.Lmain.1
.Lmain.9:
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #56]
	adrp	x0, :got:stdout
	ldr	x0, [x0, :got_lo12:stdout]
	ldr	x0, [x0]
	str	x0, [x29, #64]
	ldr	x0, [x29, #56]
	ldr	x1, [x29, #64]
	bl	fputs
	cmp	w0, #0
	b.ge	.Lmain.10
	mov	w0, #3
	b	.Lmain.1
.Lmain.10:
	adrp	x16, table+12
	ldr	w0, [x16, :lo12:table+12]
	adrp	x16, count
	ldr	w1, [x16, :lo12:count]
	add	w0, w0, w1
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
	.string	"%d squares, total %ld\n"
.Ls1:
	.string	"done\n"
	.data
	.globl	total
	.type	total, @object
	.size	total, 8
	.p2align 3
total:
	.quad	0
	.bss
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.zero	4
	.globl	table
	.type	table, @object
	.size	table, 40
	.p2align 2
table:
	.zero	40
	.section	.note.GNU-stack,"",@progbits
