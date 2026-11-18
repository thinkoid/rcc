	.file	"header-collatz.c"
	.text
	.type	halve, @function
halve:
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
	mov	x1, #2
	ldr	x2, [x0]
	sdiv	x2, x2, x1
	str	x2, [x0]
	adrp	x16, steps
	ldr	x0, [x16, :lo12:steps]
	add	x0, x0, #1
	adrp	x16, steps
	str	x0, [x16, :lo12:steps]
.Lhalve.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	halve, .-halve
	.type	triple, @function
triple:
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
	ldr	x1, [x29, #16]
	ldr	x1, [x1]
	mov	x2, #3
	mul	x1, x1, x2
	add	x1, x1, #1
	str	x1, [x0]
	adrp	x16, steps
	ldr	x0, [x16, :lo12:steps]
	add	x0, x0, #1
	adrp	x16, steps
	str	x0, [x16, :lo12:steps]
.Ltriple.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	triple, .-triple
	.type	odd, @function
odd:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, odd_values
	ldr	x0, [x16, :lo12:odd_values]
	add	x0, x0, #1
	adrp	x16, odd_values
	str	x0, [x16, :lo12:odd_values]
.Lodd.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	odd, .-odd
	.type	even, @function
even:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, even_values
	ldr	x0, [x16, :lo12:even_values]
	add	x0, x0, #1
	adrp	x16, even_values
	str	x0, [x16, :lo12:even_values]
.Leven.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	even, .-even
	.type	quarter, @function
quarter:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, quarter_values
	ldr	x0, [x16, :lo12:quarter_values]
	add	x0, x0, #1
	adrp	x16, quarter_values
	str	x0, [x16, :lo12:quarter_values]
.Lquarter.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	quarter, .-quarter
	.type	nothing, @function
nothing:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
.Lnothing.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nothing, .-nothing
	.type	record, @function
record:
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
	adrp	x16, peak
	ldr	x1, [x16, :lo12:peak]
	cmp	x0, x1
	b.le	.Lrecord.2
	ldr	x0, [x29, #16]
	adrp	x16, peak
	str	x0, [x16, :lo12:peak]
.Lrecord.2:
.Lrecord.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	record, .-record
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
	mov	x0, #27
	str	x0, [x29, #16]
.Lmain.2:
	ldr	x0, [x29, #16]
	cmp	x0, #1
	b.eq	.Lmain.4
	ldr	x0, [x29, #16]
	mov	x1, #2
	sdiv	x2, x0, x1
	msub	x0, x2, x1, x0
	cbz	x0, .Lmain.5
	bl	odd
	b	.Lmain.6
.Lmain.5:
	bl	even
.Lmain.6:
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	record
	ldr	x0, [x29, #16]
	mov	x1, #2
	sdiv	x2, x0, x1
	msub	x0, x2, x1, x0
	cbz	x0, .Lmain.7
	ldr	x0, [x29, #16]
	mov	x1, #4
	sdiv	x2, x0, x1
	msub	x0, x2, x1, x0
	cmp	x0, #1
	b.ne	.Lmain.9
	bl	quarter
	b	.Lmain.10
.Lmain.9:
	bl	nothing
.Lmain.10:
	b	.Lmain.8
.Lmain.7:
	bl	nothing
.Lmain.8:
	ldr	x0, [x29, #16]
	mov	x1, #2
	sdiv	x2, x0, x1
	msub	x0, x2, x1, x0
	cbz	x0, .Lmain.11
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	triple
	b	.Lmain.12
.Lmain.11:
	add	x0, x29, #16
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	halve
.Lmain.12:
	b	.Lmain.2
.Lmain.4:
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #48]
	adrp	x16, steps
	ldr	x0, [x16, :lo12:steps]
	str	x0, [x29, #56]
	adrp	x16, odd_values
	ldr	x0, [x16, :lo12:odd_values]
	str	x0, [x29, #64]
	adrp	x16, even_values
	ldr	x0, [x16, :lo12:even_values]
	str	x0, [x29, #72]
	adrp	x16, peak
	ldr	x0, [x16, :lo12:peak]
	str	x0, [x29, #80]
	ldr	x0, [x29, #48]
	ldr	x1, [x29, #56]
	ldr	x2, [x29, #64]
	ldr	x3, [x29, #72]
	ldr	x4, [x29, #80]
	bl	printf
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #88]
	adrp	x16, quarter_values
	ldr	x0, [x16, :lo12:quarter_values]
	str	x0, [x29, #96]
	ldr	x0, [x29, #88]
	ldr	x1, [x29, #96]
	bl	printf
	adrp	x16, steps
	ldr	x0, [x16, :lo12:steps]
	cmp	x0, #111
	b.ne	.Lmain.14
	adrp	x16, odd_values
	ldr	x0, [x16, :lo12:odd_values]
	cmp	x0, #41
	b.ne	.Lmain.14
	adrp	x16, even_values
	ldr	x0, [x16, :lo12:even_values]
	cmp	x0, #70
	b.eq	.Lmain.13
.Lmain.14:
	mov	w0, #201
	b	.Lmain.1
.Lmain.13:
	adrp	x16, peak
	ldr	x0, [x16, :lo12:peak]
	mov	x1, #9232
	cmp	x0, x1
	b.eq	.Lmain.15
	mov	w0, #202
	b	.Lmain.1
.Lmain.15:
	adrp	x16, quarter_values
	ldr	x0, [x16, :lo12:quarter_values]
	cmp	x0, #17
	b.eq	.Lmain.16
	mov	w0, #203
	b	.Lmain.1
.Lmain.16:
	adrp	x16, steps
	ldr	x0, [x16, :lo12:steps]
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
	.string	"27: %ld steps, %ld odd, %ld even, peak %ld\n"
.Ls1:
	.string	"odd values 1 modulo 4: %ld\n"
	.bss
	.type	steps, @object
	.size	steps, 8
	.p2align 3
steps:
	.zero	8
	.type	odd_values, @object
	.size	odd_values, 8
	.p2align 3
odd_values:
	.zero	8
	.type	even_values, @object
	.size	even_values, 8
	.p2align 3
even_values:
	.zero	8
	.type	quarter_values, @object
	.size	quarter_values, 8
	.p2align 3
quarter_values:
	.zero	8
	.type	peak, @object
	.size	peak, 8
	.p2align 3
peak:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
