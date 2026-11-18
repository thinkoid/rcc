	.file	"if-terminated.c"
	.text
	.globl	value
	.type	value, @function
value:
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
	cbz	w0, .Lvalue.2
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	b	.Lvalue.1
.Lvalue.2:
	ldr	w0, [x29, #20]
	mov	w1, #3
	mul	w0, w0, w1
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	b	.Lvalue.1
.Lvalue.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	value, .-value
	.globl	broken
	.type	broken, @function
broken:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
.Lbroken.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Lbroken.3
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbz	w0, .Lbroken.4
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lbroken.3
.Lbroken.4:
	ldr	w0, [x29, #20]
	add	w0, w0, #2
	str	w0, [x29, #20]
	ldr	w0, [x29, #16]
	asr	w0, w0, #1
	str	w0, [x29, #16]
	b	.Lbroken.2
.Lbroken.3:
	ldr	w0, [x29, #20]
	b	.Lbroken.1
.Lbroken.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	broken, .-broken
	.globl	continued
	.type	continued, @function
continued:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
.Lcontinued.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lcontinued.4
	ldr	w0, [x29, #24]
	and	w0, w0, #0x1
	cbz	w0, .Lcontinued.5
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	str	w0, [x29, #20]
	b	.Lcontinued.3
.Lcontinued.5:
	ldr	w0, [x29, #20]
	sub	w0, w0, #1
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
.Lcontinued.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Lcontinued.2
.Lcontinued.4:
	ldr	w0, [x29, #20]
	b	.Lcontinued.1
.Lcontinued.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	continued, .-continued
	.globl	jumped
	.type	jumped, @function
jumped:
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
	cmp	w0, #0
	b.ge	.Ljumped.2
	str	wzr, [x29, #16]
	b	.Ljumped.3
.Ljumped.2:
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	mov	w1, #2
	mul	w0, w0, w1
	str	w0, [x29, #16]
.Ljumped.3:
	ldr	w0, [x29, #16]
	b	.Ljumped.1
.Ljumped.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	jumped, .-jumped
	.globl	nested
	.type	nested, @function
nested:
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
	cbz	w0, .Lnested.2
	ldr	w0, [x29, #20]
	cbz	w0, .Lnested.3
	mov	w0, #1
	b	.Lnested.1
.Lnested.3:
	mov	w0, #2
	b	.Lnested.1
.Lnested.2:
	mov	w0, #3
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	b	.Lnested.1
.Lnested.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	fall
	.type	fall, @function
fall:
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
	cbz	w0, .Lfall.2
	ldr	w0, [x29, #20]
	cbz	w0, .Lfall.3
	mov	w0, #1
	b	.Lfall.1
.Lfall.3:
	b	.Lfall.4
.Lfall.2:
	mov	w0, #3
	str	w0, [x29, #24]
.Lfall.4:
	ldr	w0, [x29, #24]
	b	.Lfall.1
.Lfall.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fall, .-fall
	.globl	landed
	.type	landed, @function
landed:
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
	cbz	w0, .Llanded.2
	ldr	w0, [x29, #20]
	cmp	w0, #9
	b.gt	.Llanded.3
	ldr	w0, [x29, #20]
	b	.Llanded.1
.Llanded.3:
	b	.Llanded.4
.Llanded.2:
	mov	w0, #2
	str	w0, [x29, #20]
.Llanded.4:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	b	.Llanded.1
.Llanded.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	landed, .-landed
	.section	.note.GNU-stack,"",@progbits
