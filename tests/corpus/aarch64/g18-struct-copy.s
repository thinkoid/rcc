	.file	"g18-struct-copy.c"
	.text
	.globl	at
	.type	at, @function
at:
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
	add	x0, x0, w1, sxtw #2
	b	.Lat.1
.Lat.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	at, .-at
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
	cmp	w0, #8
	b.ge	.Lfill.4
	adrp	x0, codes
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #2
	ldr	w1, [x29, #16]
	mov	w2, #7
	mul	w1, w1, w2
	add	w1, w1, #1
	strb	w1, [x0, :lo12:codes]
	adrp	x0, codes+1
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #2
	ldr	w1, [x29, #16]
	add	w1, w1, #3
	strb	w1, [x0, :lo12:codes+1]
	adrp	x0, codes+2
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #2
	ldr	w1, [x29, #16]
	mov	w2, #1000
	mul	w1, w1, w2
	add	w1, w1, #17
	strh	w1, [x0, :lo12:codes+2]
	adrp	x0, points
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #3
	ldr	w1, [x29, #16]
	mov	w2, #11
	mul	w1, w1, w2
	sub	w1, w1, #5
	str	w1, [x0, :lo12:points]
	adrp	x0, points+4
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #3
	ldr	w1, [x29, #16]
	ldr	w2, [x29, #16]
	mul	w1, w1, w2
	str	w1, [x0, :lo12:points+4]
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lfill.2
.Lfill.4:
	str	wzr, [x29, #16]
.Lfill.5:
	ldr	w0, [x29, #16]
	cmp	w0, #4
	b.ge	.Lfill.7
	str	wzr, [x29, #20]
.Lfill.8:
	ldr	w0, [x29, #20]
	cmp	w0, #32
	b.ge	.Lfill.10
	adrp	x0, texts
	ldrsw	x1, [x29, #16]
	add	x0, x0, x1, lsl #5
	ldr	w1, [x29, #20]
	add	x0, x0, w1, sxtw
	ldr	w1, [x29, #16]
	mov	w2, #32
	mul	w1, w1, w2
	ldr	w2, [x29, #20]
	add	w1, w1, w2
	strb	w1, [x0, :lo12:texts]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lfill.8
.Lfill.10:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lfill.5
.Lfill.7:
	str	wzr, [x29, #16]
.Lfill.11:
	ldr	w0, [x29, #16]
	cmp	w0, #4
	b.ge	.Lfill.13
	str	wzr, [x29, #20]
.Lfill.14:
	ldr	w0, [x29, #20]
	cmp	w0, #9
	b.ge	.Lfill.16
	adrp	x0, nines
	ldrsw	x1, [x29, #16]
	mov	x2, #72
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w1, [x29, #20]
	add	x0, x0, w1, sxtw #3
	ldrsw	x1, [x29, #16]
	mov	x2, #16963
	movk	x2, #0xf, lsl #16
	mul	x1, x1, x2
	ldrsw	x2, [x29, #20]
	add	x1, x1, x2
	str	x1, [x0, :lo12:nines]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lfill.14
.Lfill.16:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lfill.11
.Lfill.13:
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
	.globl	rotate
	.type	rotate, @function
rotate:
	.cfi_startproc
	sub	sp, sp, #144
	.cfi_def_cfa_offset 144
	stp	x29, x30, [sp]
	.cfi_offset w29, -144
	.cfi_offset w30, -136
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	ldrh	w17, [x0]
	strh	w17, [x29, #16]
	ldrh	w17, [x0, #2]
	strh	w17, [x29, #18]
	str	wzr, [x29, #64]
.Lrotate.2:
	ldr	w0, [x29, #64]
	cmp	w0, #7
	b.ge	.Lrotate.4
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	ldr	w1, [x29, #64]
	adrp	x2, codes
	add	x2, x2, :lo12:codes
	ldr	w3, [x29, #64]
	add	w3, w3, #1
	add	x0, x0, w1, sxtw #2
	add	x2, x2, w3, sxtw #2
	ldrh	w17, [x2]
	strh	w17, [x0]
	ldrh	w17, [x2, #2]
	strh	w17, [x0, #2]
	ldr	w0, [x29, #64]
	add	w0, w0, #1
	str	w0, [x29, #64]
	b	.Lrotate.2
.Lrotate.4:
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	ldrh	w17, [x29, #16]
	strh	w17, [x0, #28]
	ldrh	w17, [x29, #18]
	strh	w17, [x0, #30]
	adrp	x0, points
	add	x0, x0, :lo12:points
	str	x0, [x29, #56]
.Lrotate.5:
	ldr	x0, [x29, #56]
	adrp	x1, points+56
	add	x1, x1, :lo12:points+56
	cmp	x0, x1
	b.hs	.Lrotate.7
	ldr	x0, [x29, #56]
	ldr	x1, [x29, #56]
	ldr	w17, [x1, #8]
	str	w17, [x0]
	ldr	w17, [x1, #12]
	str	w17, [x0, #4]
	ldr	x0, [x29, #56]
	add	x0, x0, #8
	str	x0, [x29, #56]
	b	.Lrotate.5
.Lrotate.7:
	adrp	x0, nines
	add	x0, x0, :lo12:nines
	adrp	x1, spare
	add	x1, x1, :lo12:spare
	mov	x16, #9
.Lrotate.8:
	ldr	x17, [x0], #8
	str	x17, [x1], #8
	subs	x16, x16, #1
	b.ne	.Lrotate.8
	str	wzr, [x29, #64]
.Lrotate.9:
	ldr	w0, [x29, #64]
	cmp	w0, #3
	b.ge	.Lrotate.11
	adrp	x0, nines
	add	x0, x0, :lo12:nines
	ldrsw	x1, [x29, #64]
	mov	x2, #72
	mul	x1, x1, x2
	add	x0, x0, x1
	adrp	x1, nines
	add	x1, x1, :lo12:nines
	ldr	w2, [x29, #64]
	add	w2, w2, #1
	sxtw	x2, w2
	mov	x3, #72
	mul	x2, x2, x3
	add	x1, x1, x2
	mov	x16, #9
.Lrotate.12:
	ldr	x17, [x1], #8
	str	x17, [x0], #8
	subs	x16, x16, #1
	b.ne	.Lrotate.12
	ldr	w0, [x29, #64]
	add	w0, w0, #1
	str	w0, [x29, #64]
	b	.Lrotate.9
.Lrotate.11:
	adrp	x0, nines
	add	x0, x0, :lo12:nines
	add	x0, x0, #216
	adrp	x1, spare
	add	x1, x1, :lo12:spare
	mov	x16, #9
.Lrotate.13:
	ldr	x17, [x1], #8
	str	x17, [x0], #8
	subs	x16, x16, #1
	b.ne	.Lrotate.13
	adrp	x0, texts
	add	x0, x0, :lo12:texts
	add	x1, x29, #20
	add	x0, x0, #96
	mov	x16, #32
.Lrotate.14:
	ldrb	w17, [x0], #1
	strb	w17, [x1], #1
	subs	x16, x16, #1
	b.ne	.Lrotate.14
	mov	w0, #3
	str	w0, [x29, #64]
.Lrotate.15:
	ldr	w0, [x29, #64]
	cmp	w0, #0
	b.le	.Lrotate.17
	adrp	x0, texts
	add	x0, x0, :lo12:texts
	ldrsw	x1, [x29, #64]
	add	x0, x0, x1, lsl #5
	adrp	x1, texts
	add	x1, x1, :lo12:texts
	ldr	w2, [x29, #64]
	sub	w2, w2, #1
	sxtw	x2, w2
	add	x1, x1, x2, lsl #5
	mov	x16, #32
.Lrotate.18:
	ldrb	w17, [x1], #1
	strb	w17, [x0], #1
	subs	x16, x16, #1
	b.ne	.Lrotate.18
	ldr	w0, [x29, #64]
	sub	w0, w0, #1
	str	w0, [x29, #64]
	b	.Lrotate.15
.Lrotate.17:
	adrp	x0, texts
	add	x0, x0, :lo12:texts
	add	x1, x29, #20
	mov	x16, #32
.Lrotate.19:
	ldrb	w17, [x1], #1
	strb	w17, [x0], #1
	subs	x16, x16, #1
	b.ne	.Lrotate.19
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	str	x0, [x29, #72]
	mov	w0, #2
	str	w0, [x29, #80]
	ldr	x0, [x29, #72]
	ldr	w1, [x29, #80]
	bl	at
	ldrh	w17, [x0]
	strh	w17, [x29, #16]
	ldrh	w17, [x0, #2]
	strh	w17, [x29, #18]
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	str	x0, [x29, #88]
	mov	w0, #2
	str	w0, [x29, #96]
	ldr	x0, [x29, #88]
	ldr	w1, [x29, #96]
	bl	at
	str	x0, [x29, #104]
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	str	x0, [x29, #112]
	mov	w0, #5
	str	w0, [x29, #120]
	ldr	x0, [x29, #112]
	ldr	w1, [x29, #120]
	bl	at
	ldrh	w17, [x0]
	ldr	x1, [x29, #104]
	strh	w17, [x1]
	ldrh	w17, [x0, #2]
	strh	w17, [x1, #2]
	adrp	x0, codes
	add	x0, x0, :lo12:codes
	str	x0, [x29, #128]
	mov	w0, #5
	str	w0, [x29, #136]
	ldr	x0, [x29, #128]
	ldr	w1, [x29, #136]
	bl	at
	ldrh	w17, [x29, #16]
	strh	w17, [x0]
	ldrh	w17, [x29, #18]
	strh	w17, [x0, #2]
.Lrotate.1:
	.cfi_def_cfa wsp, 144
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #144
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rotate, .-rotate
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	xzr, [x29, #16]
	str	wzr, [x29, #24]
.Lsum.2:
	ldr	w0, [x29, #24]
	cmp	w0, #8
	b.ge	.Lsum.4
	ldr	x0, [x29, #16]
	mov	x1, #31
	mul	x0, x0, x1
	adrp	x1, codes
	ldr	w2, [x29, #24]
	add	x1, x1, w2, sxtw #2
	ldrb	w1, [x1, :lo12:codes]
	add	x0, x0, x1
	adrp	x1, codes+1
	ldr	w2, [x29, #24]
	add	x1, x1, w2, sxtw #2
	ldrb	w1, [x1, :lo12:codes+1]
	mov	w2, #3
	mul	w1, w1, w2
	sxtw	x1, w1
	add	x0, x0, x1
	adrp	x1, codes+2
	ldr	w2, [x29, #24]
	add	x1, x1, w2, sxtw #2
	ldrh	w1, [x1, :lo12:codes+2]
	mov	w2, #7
	mul	w1, w1, w2
	sxtw	x1, w1
	add	x0, x0, x1
	adrp	x1, points
	ldr	w2, [x29, #24]
	add	x1, x1, w2, sxtw #3
	ldrsw	x1, [x1, :lo12:points]
	mov	x2, #5
	mul	x1, x1, x2
	add	x0, x0, x1
	adrp	x1, points+4
	ldr	w2, [x29, #24]
	add	x1, x1, w2, sxtw #3
	ldrsw	x1, [x1, :lo12:points+4]
	add	x0, x0, x1
	str	x0, [x29, #16]
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lsum.2
.Lsum.4:
	str	wzr, [x29, #24]
.Lsum.5:
	ldr	w0, [x29, #24]
	cmp	w0, #4
	b.ge	.Lsum.7
	str	wzr, [x29, #28]
.Lsum.8:
	ldr	w0, [x29, #28]
	cmp	w0, #32
	b.ge	.Lsum.10
	ldr	x0, [x29, #16]
	mov	x1, #31
	mul	x0, x0, x1
	adrp	x1, texts
	ldrsw	x2, [x29, #24]
	add	x1, x1, x2, lsl #5
	ldr	w2, [x29, #28]
	add	x1, x1, w2, sxtw
	ldrb	w1, [x1, :lo12:texts]
	add	x0, x0, x1
	str	x0, [x29, #16]
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lsum.8
.Lsum.10:
	str	wzr, [x29, #28]
.Lsum.11:
	ldr	w0, [x29, #28]
	cmp	w0, #9
	b.ge	.Lsum.13
	ldr	x0, [x29, #16]
	mov	x1, #31
	mul	x0, x0, x1
	adrp	x1, nines
	ldrsw	x2, [x29, #24]
	mov	x3, #72
	mul	x2, x2, x3
	add	x1, x1, x2
	ldr	w2, [x29, #28]
	add	x1, x1, w2, sxtw #3
	ldr	x1, [x1, :lo12:nines]
	add	x0, x0, x1
	str	x0, [x29, #16]
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lsum.11
.Lsum.13:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lsum.5
.Lsum.7:
	ldr	x0, [x29, #16]
	b	.Lsum.1
.Lsum.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	fill
	bl	rotate
	bl	rotate
	bl	sum
	mov	x1, #251
	udiv	x2, x0, x1
	msub	x0, x2, x1, x0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	codes
	.type	codes, @object
	.size	codes, 32
	.p2align 1
codes:
	.zero	32
	.globl	points
	.type	points, @object
	.size	points, 64
	.p2align 2
points:
	.zero	64
	.globl	texts
	.type	texts, @object
	.size	texts, 128
texts:
	.zero	128
	.globl	nines
	.type	nines, @object
	.size	nines, 288
	.p2align 3
nines:
	.zero	288
	.globl	spare
	.type	spare, @object
	.size	spare, 72
	.p2align 3
spare:
	.zero	72
	.section	.note.GNU-stack,"",@progbits
