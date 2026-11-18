	.file	"enum.c"
	.text
	.globl	next
	.type	next, @function
next:
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
	cmp	w0, #1
	b.hi	.Lnext.4
	adrp	x16, .Lnext.6
	add	x16, x16, :lo12:.Lnext.6
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Lnext.6:
	.quad	.Lnext.2
	.quad	.Lnext.3
	.text
.Lnext.2:
	mov	w0, #1
	b	.Lnext.1
.Lnext.3:
	mov	w0, #2
	b	.Lnext.1
.Lnext.4:
	mov	w0, #0
	b	.Lnext.1
.Lnext.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	signs
	.type	signs, @function
signs:
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
	ldr	w0, [x29, #16]
	sub	w0, w0, #1
	cmp	w0, #0
	cset	w0, lo
	ldr	w1, [x29, #20]
	cmp	w1, #0
	cset	w1, lt
	mov	w2, #2
	mul	w1, w1, w2
	add	w0, w0, w1
	add	w0, w0, #8
	b	.Lsigns.1
.Lsigns.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	signs, .-signs
	.globl	widen
	.type	widen, @function
widen:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	w0, [x0]
	str	w0, [x29, #32]
	ldr	x0, [x29, #16]
	str	w0, [x29, #36]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #32]
	add	x0, x0, x1
	ldr	w1, [x29, #36]
	add	x0, x0, x1
	mov	x1, #4294967296
	add	x0, x0, x1
	add	x0, x0, #-2
	b	.Lwiden.1
.Lwiden.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widen, .-widen
	.section	.note.GNU-stack,"",@progbits
