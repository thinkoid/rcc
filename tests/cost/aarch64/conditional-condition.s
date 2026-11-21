	.file	"conditional-condition.c"
	.text
	.globl	joined
	.type	joined, @function
joined:
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
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	cbz	w0, .Ljoined.3
	ldr	w0, [x29, #20]
	b	.Ljoined.4
.Ljoined.3:
	ldr	w0, [x29, #24]
.Ljoined.4:
	cbz	w0, .Ljoined.2
	bl	g
.Ljoined.2:
.Ljoined.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	joined, .-joined
	.globl	compared
	.type	compared, @function
compared:
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
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	ldr	w0, [x29, #16]
	cbz	w0, .Lcompared.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lcompared.2
	b	.Lcompared.4
.Lcompared.3:
	ldr	w0, [x29, #28]
	cbz	w0, .Lcompared.2
.Lcompared.4:
	bl	g
.Lcompared.2:
.Lcompared.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compared, .-compared
	.globl	called
	.type	called, @function
called:
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
	cbz	w0, .Lcalled.3
	bl	f
	b	.Lcalled.4
.Lcalled.3:
	ldr	w0, [x29, #20]
.Lcalled.4:
	cbz	w0, .Lcalled.2
	bl	g
.Lcalled.2:
.Lcalled.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	negated
	.type	negated, @function
negated:
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
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	ldr	w0, [x29, #16]
	cbz	w0, .Lnegated.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.lt	.Lnegated.2
	b	.Lnegated.4
.Lnegated.3:
	ldr	w0, [x29, #28]
	cbnz	w0, .Lnegated.2
.Lnegated.4:
	bl	g
.Lnegated.2:
.Lnegated.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	negated, .-negated
	.globl	looped
	.type	looped, @function
looped:
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
	str	w2, [x29, #24]
	str	w3, [x29, #28]
.Llooped.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Llooped.4
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Llooped.3
	b	.Llooped.5
.Llooped.4:
	ldr	w0, [x29, #28]
	cbz	w0, .Llooped.3
.Llooped.5:
	bl	g
	b	.Llooped.2
.Llooped.3:
.Llooped.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	looped, .-looped
	.globl	both
	.type	both, @function
both:
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
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	ldr	w0, [x29, #16]
	cbz	w0, .Lboth.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lboth.2
	b	.Lboth.4
.Lboth.3:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #28]
	cmp	w0, w1
	b.le	.Lboth.2
.Lboth.4:
	bl	g
.Lboth.2:
.Lboth.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	held
	.type	held, @function
held:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	ldr	w0, [x29, #28]
	str	x0, [x29, #32]
	ldr	w0, [x29, #16]
	cbz	w0, .Lheld.4
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lheld.2
	b	.Lheld.5
.Lheld.4:
	ldr	w0, [x29, #28]
	cbz	w0, .Lheld.2
.Lheld.5:
	mov	w0, #1
	b	.Lheld.3
.Lheld.2:
	mov	w0, #2
.Lheld.3:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	b	.Lheld.1
.Lheld.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	written
	.type	written, @function
written:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w19, [x29, #32]
	str	x19, [x29, #40]
	ldr	w0, [x29, #24]
	cbz	w0, .Lwritten.4
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #32]
	cmp	w19, w0
	b.ge	.Lwritten.2
	b	.Lwritten.5
.Lwritten.4:
	cbz	w19, .Lwritten.2
.Lwritten.5:
	mov	w0, #1
	b	.Lwritten.3
.Lwritten.2:
	mov	w0, #2
.Lwritten.3:
	ldr	x1, [x29, #40]
	add	w0, w1, w0
	b	.Lwritten.1
.Lwritten.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	written, .-written
	.globl	converted
	.type	converted, @function
converted:
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
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	ldr	w0, [x29, #16]
	cbz	w0, .Lconverted.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lconverted.2
	b	.Lconverted.4
.Lconverted.3:
	ldr	w0, [x29, #28]
	cbz	w0, .Lconverted.2
.Lconverted.4:
	bl	g
.Lconverted.2:
.Lconverted.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	converted, .-converted
	.globl	narrow
	.type	narrow, @function
narrow:
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
	str	w2, [x29, #24]
	strb	w3, [x29, #28]
	ldr	w0, [x29, #16]
	cbz	w0, .Lnarrow.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lnarrow.2
	b	.Lnarrow.4
.Lnarrow.3:
	ldrb	w0, [x29, #28]
	cbz	w0, .Lnarrow.2
.Lnarrow.4:
	bl	g
.Lnarrow.2:
.Lnarrow.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.section	.note.GNU-stack,"",@progbits
