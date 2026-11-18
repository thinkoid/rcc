	.file	"switch-hash.c"
	.text
	.globl	reason
	.type	reason, @function
reason:
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
	mov	w1, #5
	udiv	w2, w0, w1
	msub	w0, w2, w1, w0
	adrp	x16, .Lreason.16
	add	x16, x16, :lo12:.Lreason.16
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Lreason.16:
	.quad	.Lreason.17
	.quad	.Lreason.18
	.quad	.Lreason.19
	.quad	.Lreason.20
	.quad	.Lreason.21
	.text
.Lreason.17:
	cmp	w2, #40
	b.eq	.Lreason.2
	cmp	w2, #80
	b.eq	.Lreason.8
	cmp	w2, #100
	b.eq	.Lreason.12
	b	.Lreason.14
.Lreason.18:
	cmp	w2, #40
	b.eq	.Lreason.3
	cmp	w2, #60
	b.eq	.Lreason.5
	cmp	w2, #80
	b.eq	.Lreason.9
	b	.Lreason.14
.Lreason.19:
	cmp	w2, #60
	b.eq	.Lreason.6
	b	.Lreason.14
.Lreason.20:
	cmp	w2, #80
	b.eq	.Lreason.10
	cmp	w2, #100
	b.eq	.Lreason.13
	b	.Lreason.14
.Lreason.21:
	cmp	w2, #40
	b.eq	.Lreason.4
	cmp	w2, #60
	b.eq	.Lreason.7
	cmp	w2, #80
	b.eq	.Lreason.11
	b	.Lreason.14
.Lreason.2:
	mov	w0, #1
	b	.Lreason.1
.Lreason.3:
	mov	w0, #2
	b	.Lreason.1
.Lreason.4:
	mov	w0, #3
	b	.Lreason.1
.Lreason.5:
	mov	w0, #4
	b	.Lreason.1
.Lreason.6:
	mov	w0, #5
	b	.Lreason.1
.Lreason.7:
	mov	w0, #6
	b	.Lreason.1
.Lreason.8:
	mov	w0, #7
	b	.Lreason.1
.Lreason.9:
	mov	w0, #8
	b	.Lreason.1
.Lreason.10:
	mov	w0, #9
	b	.Lreason.1
.Lreason.11:
	mov	w0, #10
	b	.Lreason.1
.Lreason.12:
	mov	w0, #11
	b	.Lreason.1
.Lreason.13:
	mov	w0, #12
	b	.Lreason.1
.Lreason.14:
	mov	w0, #0
	b	.Lreason.1
.Lreason.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	reason, .-reason
	.section	.note.GNU-stack,"",@progbits
