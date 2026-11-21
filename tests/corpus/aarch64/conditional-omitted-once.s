	.file	"conditional-omitted-once.c"
	.text
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	add	w0, w0, #1
	adrp	x16, calls
	str	w0, [x16, :lo12:calls]
	adrp	x16, counter
	ldr	w0, [x16, :lo12:counter]
	add	w1, w0, #1
	adrp	x16, counter
	str	w1, [x16, :lo12:counter]
	b	.Lnext.1
.Lnext.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -48
	bl	next
	cbnz	w0, .Lmain.4
	mov	w0, #7
.Lmain.4:
	cmp	w0, #7
	b.ne	.Lmain.3
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	cmp	w0, #1
	b.eq	.Lmain.2
.Lmain.3:
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	bl	next
	cbnz	w0, .Lmain.7
	mov	w0, #7
.Lmain.7:
	cmp	w0, #1
	b.ne	.Lmain.6
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	cmp	w0, #2
	b.eq	.Lmain.5
.Lmain.6:
	mov	w0, #102
	b	.Lmain.1
.Lmain.5:
	mov	w0, #-3
	strb	w0, [x29, #24]
	ldrsb	w0, [x29, #24]
	cbnz	w0, .Lmain.9
	mov	w0, #9
.Lmain.9:
	cmp	w0, #-3
	b.eq	.Lmain.8
	mov	w0, #103
	b	.Lmain.1
.Lmain.8:
	strb	wzr, [x29, #24]
	ldrsb	w0, [x29, #24]
	cbnz	w0, .Lmain.11
	mov	w0, #9
.Lmain.11:
	cmp	w0, #9
	b.eq	.Lmain.10
	mov	w0, #104
	b	.Lmain.1
.Lmain.10:
	str	xzr, [x29, #32]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #40]
	ldr	x0, [x29, #32]
	cbnz	x0, .Lmain.14
	ldr	x0, [x29, #40]
.Lmain.14:
	ldr	x1, [x29, #40]
	cmp	x0, x1
	b.ne	.Lmain.13
	ldr	x0, [x29, #40]
	cbnz	x0, .Lmain.15
	ldr	x0, [x29, #32]
.Lmain.15:
	ldr	x1, [x29, #40]
	cmp	x0, x1
	b.eq	.Lmain.12
.Lmain.13:
	mov	w0, #105
	b	.Lmain.1
.Lmain.12:
	bl	next
	mov	w19, w0
	cbnz	w19, .Lmain.16
	mov	w19, #5
.Lmain.16:
	cmp	w19, #2
	b.ne	.Lmain.18
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	cmp	w0, #3
	b.eq	.Lmain.17
.Lmain.18:
	mov	w0, #106
	b	.Lmain.1
.Lmain.17:
	adrp	x16, counter
	str	wzr, [x16, :lo12:counter]
	bl	next
	cbnz	w0, .Lmain.20
	mov	w0, #0
.Lmain.20:
	cbz	w0, .Lmain.19
	mov	w0, #107
	b	.Lmain.1
.Lmain.19:
	adrp	x16, counter
	str	wzr, [x16, :lo12:counter]
	mov	w0, #0
	cbnz	w0, .Lmain.23
	bl	next
	cbnz	w0, .Lmain.24
	mov	w0, #8
.Lmain.24:
.Lmain.23:
	cmp	w0, #8
	b.ne	.Lmain.22
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	cmp	w0, #5
	b.eq	.Lmain.21
.Lmain.22:
	mov	w0, #108
	b	.Lmain.1
.Lmain.21:
	mov	w0, #-5
	str	w0, [x29, #48]
	ldrsw	x0, [x29, #48]
	cbnz	x0, .Lmain.25
	mov	x0, #3
.Lmain.25:
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	cmp	x0, #-5
	b.eq	.Lmain.26
	mov	w0, #109
	b	.Lmain.1
.Lmain.26:
	bl	next
	cbnz	w0, .Lmain.27
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	add	w0, w0, #10
	adrp	x16, calls
	str	w0, [x16, :lo12:calls]
.Lmain.27:
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	cmp	w0, #6
	b.eq	.Lmain.28
	mov	w0, #110
	b	.Lmain.1
.Lmain.28:
	adrp	x16, counter
	str	wzr, [x16, :lo12:counter]
	bl	next
	cbnz	w0, .Lmain.29
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	add	w0, w0, #10
	adrp	x16, calls
	str	w0, [x16, :lo12:calls]
.Lmain.29:
	adrp	x16, calls
	ldr	w0, [x16, :lo12:calls]
	cmp	w0, #17
	b.eq	.Lmain.30
	mov	w0, #111
	b	.Lmain.1
.Lmain.30:
	mov	w0, #42
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 64
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"q"
	.bss
	.globl	counter
	.type	counter, @object
	.size	counter, 4
	.p2align 2
counter:
	.zero	4
	.globl	calls
	.type	calls, @object
	.size	calls, 4
	.p2align 2
calls:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
