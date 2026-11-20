	.file	"header-grids.c"
	.text
	.globl	number
	.type	number, @function
number:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, count
	ldr	w0, [x16, :lo12:count]
	add	w0, w0, #1
	adrp	x16, count
	str	w0, [x16, :lo12:count]
	b	.Lnumber.1
.Lnumber.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	number, .-number
	.globl	fill
	.type	fill, @function
fill:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
.Lfill.2:
	ldr	w0, [x29, #16]
	cmp	w0, #4
	b.ge	.Lfill.4
	str	wzr, [x29, #20]
.Lfill.5:
	ldr	w0, [x29, #20]
	cmp	w0, #4
	b.ge	.Lfill.7
	adrp	x0, grid
	add	x0, x0, :lo12:grid
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #4
	ldr	w1, [x29, #20]
	add	x0, x0, w1, sxtw #2
	str	x0, [x29, #24]
	bl	number
	ldr	x1, [x29, #24]
	str	w0, [x1]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lfill.5
.Lfill.7:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lfill.2
.Lfill.4:
	str	wzr, [x29, #16]
.Lfill.8:
	ldr	w0, [x29, #16]
	cmp	w0, #4
	b.ge	.Lfill.10
	str	wzr, [x29, #20]
.Lfill.11:
	ldr	w0, [x29, #20]
	cmp	w0, #4
	b.ge	.Lfill.13
	adrp	x0, grid
	add	x0, x0, :lo12:grid
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #4
	ldr	w1, [x29, #20]
	ldr	w0, [x0, w1, sxtw #2]
	ldr	w1, [x29, #16]
	mov	w2, #4
	mul	w1, w1, w2
	ldr	w2, [x29, #20]
	add	w1, w1, w2
	add	w1, w1, #1
	cmp	w0, w1
	b.eq	.Lfill.14
	mov	w0, #1
	b	.Lfill.1
.Lfill.14:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lfill.11
.Lfill.13:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lfill.8
.Lfill.10:
	mov	w0, #0
	b	.Lfill.1
.Lfill.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fill, .-fill
	.globl	numbered
	.type	numbered, @function
numbered:
	.cfi_startproc
	sub	sp, sp, #112
	.cfi_def_cfa_offset 112
	stp	x29, x30, [sp]
	.cfi_offset w29, -112
	.cfi_offset w30, -104
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #80]
.Lnumbered.2:
	ldr	w0, [x29, #80]
	cmp	w0, #4
	b.ge	.Lnumbered.4
	str	wzr, [x29, #84]
.Lnumbered.5:
	ldr	w0, [x29, #84]
	cmp	w0, #4
	b.ge	.Lnumbered.7
	add	x0, x29, #16
	ldr	w1, [x29, #80]
	add	x0, x0, w1, sxtw #4
	ldr	w1, [x29, #84]
	add	x0, x0, w1, sxtw #2
	str	x0, [x29, #96]
	bl	number
	ldr	x1, [x29, #96]
	str	w0, [x1]
	ldr	w0, [x29, #84]
	add	w0, w0, #1
	str	w0, [x29, #84]
	b	.Lnumbered.5
.Lnumbered.7:
	ldr	w0, [x29, #80]
	add	w0, w0, #1
	str	w0, [x29, #80]
	b	.Lnumbered.2
.Lnumbered.4:
	str	wzr, [x29, #88]
	str	wzr, [x29, #80]
.Lnumbered.8:
	ldr	w0, [x29, #80]
	cmp	w0, #4
	b.ge	.Lnumbered.10
	add	x0, x29, #16
	ldr	w1, [x29, #80]
	add	x0, x0, w1, sxtw #4
	ldr	w1, [x29, #80]
	ldr	w0, [x0, w1, sxtw #2]
	ldr	w1, [x29, #88]
	add	w1, w1, w0
	str	w1, [x29, #88]
	ldr	w0, [x29, #80]
	add	w0, w0, #1
	str	w0, [x29, #80]
	b	.Lnumbered.8
.Lnumbered.10:
	ldr	w0, [x29, #88]
	b	.Lnumbered.1
.Lnumbered.1:
	.cfi_def_cfa wsp, 112
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #112
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	numbered, .-numbered
	.globl	layer
	.type	layer, @function
layer:
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
	adrp	x0, cube
	add	x0, x0, :lo12:cube
	ldrsw	x1, [x29, #16]
	mov	x2, #36
	mul	x1, x1, x2
	add	x0, x0, x1
	ldrsw	x1, [x29, #20]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	b	.Llayer.1
.Llayer.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	layer, .-layer
	.globl	back
	.type	back, @function
back:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	sub	x0, x0, w1, sxtw #2
	ldr	w0, [x0, #16]
	b	.Lback.1
.Lback.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	back, .-back
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #96
	.cfi_def_cfa_offset 96
	stp	x29, x30, [sp]
	.cfi_offset w29, -96
	.cfi_offset w30, -88
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	fill
	cmp	w0, #0
	b.eq	.Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	bl	numbered
	cmp	w0, #98
	b.eq	.Lmain.3
	mov	w0, #2
	b	.Lmain.1
.Lmain.3:
	str	wzr, [x29, #16]
.Lmain.4:
	ldr	w0, [x29, #16]
	cmp	w0, #3
	b.ge	.Lmain.6
	str	wzr, [x29, #20]
.Lmain.7:
	ldr	w0, [x29, #20]
	cmp	w0, #3
	b.ge	.Lmain.9
	str	wzr, [x29, #24]
.Lmain.10:
	ldr	w0, [x29, #24]
	cmp	w0, #3
	b.ge	.Lmain.12
	adrp	x0, cube
	add	x0, x0, :lo12:cube
	ldrsw	x1, [x29, #16]
	mov	x2, #36
	mul	x1, x1, x2
	add	x0, x0, x1
	ldrsw	x1, [x29, #20]
	mov	x2, #12
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w1, [x29, #24]
	ldr	w2, [x29, #16]
	mov	w3, #9
	mul	w2, w2, w3
	ldr	w3, [x29, #20]
	mov	w4, #3
	mul	w3, w3, w4
	add	w2, w2, w3
	ldr	w3, [x29, #24]
	add	w2, w2, w3
	str	w2, [x0, w1, sxtw #2]
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lmain.10
.Lmain.12:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lmain.7
.Lmain.9:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lmain.4
.Lmain.6:
	mov	w0, #2
	str	w0, [x29, #28]
	mov	w0, #1
	str	w0, [x29, #32]
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #32]
	bl	layer
	cmp	w0, #23
	b.eq	.Lmain.13
	mov	w0, #3
	b	.Lmain.1
.Lmain.13:
	adrp	x0, grid+32
	add	x0, x0, :lo12:grid+32
	str	x0, [x29, #40]
	mov	w0, #2
	str	w0, [x29, #48]
	ldr	x0, [x29, #40]
	ldr	w1, [x29, #48]
	bl	back
	cmp	w0, #11
	b.eq	.Lmain.14
	mov	w0, #4
	b	.Lmain.1
.Lmain.14:
	adrp	x16, grid+24
	ldr	w0, [x16, :lo12:grid+24]
	str	x0, [x29, #56]
	mov	w0, #1
	str	w0, [x29, #64]
	mov	w0, #2
	str	w0, [x29, #68]
	ldr	w0, [x29, #64]
	ldr	w1, [x29, #68]
	bl	layer
	ldr	x1, [x29, #56]
	add	w1, w1, w0
	str	x1, [x29, #72]
	adrp	x0, grid+32
	add	x0, x0, :lo12:grid+32
	str	x0, [x29, #80]
	mov	w0, #2
	str	w0, [x29, #88]
	ldr	x0, [x29, #80]
	ldr	w1, [x29, #88]
	bl	back
	ldr	x1, [x29, #72]
	add	w0, w1, w0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 96
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #96
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	grid
	.type	grid, @object
	.size	grid, 64
	.p2align 2
grid:
	.zero	64
	.globl	cube
	.type	cube, @object
	.size	cube, 108
	.p2align 2
cube:
	.zero	108
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
