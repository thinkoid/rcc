	.file	"if-jump.c"
	.text
	.globl	low
	.type	low, @function
low:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Llow.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Llow.3
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbnz	w0, .Llow.3
	ldr	w0, [x29, #16]
	asr	w0, w0, #1
	str	w0, [x29, #16]
	b	.Llow.2
.Llow.3:
	ldr	w0, [x29, #16]
	b	.Llow.1
.Llow.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	low, .-low
	.globl	low_else
	.type	low_else, @function
low_else:
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
.Llow_else.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Llow_else.3
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbnz	w0, .Llow_else.3
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	ldr	w0, [x29, #16]
	asr	w0, w0, #1
	str	w0, [x29, #16]
	b	.Llow_else.2
.Llow_else.3:
	ldr	w0, [x29, #20]
	b	.Llow_else.1
.Llow_else.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	low_else, .-low_else
	.globl	even
	.type	even, @function
even:
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
.Leven.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Leven.4
	ldr	w0, [x29, #24]
	and	w0, w0, #0x1
	cbnz	w0, .Leven.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	str	w0, [x29, #20]
.Leven.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Leven.2
.Leven.4:
	ldr	w0, [x29, #20]
	b	.Leven.1
.Leven.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	even, .-even
	.globl	even_else
	.type	even_else, @function
even_else:
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
.Leven_else.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Leven_else.4
	ldr	w0, [x29, #24]
	and	w0, w0, #0x1
	cbnz	w0, .Leven_else.3
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
.Leven_else.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Leven_else.2
.Leven_else.4:
	ldr	w0, [x29, #20]
	b	.Leven_else.1
.Leven_else.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	even_else, .-even_else
	.globl	clamp
	.type	clamp, @function
clamp:
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
	b.ge	.Lclamp.2
	str	wzr, [x29, #16]
.Lclamp.2:
	ldr	w0, [x29, #16]
	b	.Lclamp.1
.Lclamp.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	clamp, .-clamp
	.globl	clamp_else
	.type	clamp_else, @function
clamp_else:
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
	b.ge	.Lclamp_else.2
	str	wzr, [x29, #16]
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
.Lclamp_else.2:
	ldr	w0, [x29, #16]
	b	.Lclamp_else.1
.Lclamp_else.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	clamp_else, .-clamp_else
	.globl	early
	.type	early, @function
early:
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
	ldr	w0, [x0]
	cbnz	w0, .Learly.1
	ldr	x0, [x29, #16]
	mov	w1, #1
	str	w1, [x0]
.Learly.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	early, .-early
	.globl	early_else
	.type	early_else, @function
early_else:
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
	ldr	w0, [x0]
	cbnz	w0, .Learly_else.1
	ldr	x0, [x29, #16]
	mov	w1, #1
	str	w1, [x0]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #16]
	ldr	w1, [x1]
	add	w1, w1, #1
	str	w1, [x0]
.Learly_else.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	early_else, .-early_else
	.globl	tally
	.type	tally, @function
tally:
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
.Ltally.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Ltally.4
	ldr	w0, [x29, #24]
	and	w0, w0, #0x3
	cmp	w0, #1
	b.hi	.Ltally.7
	adrp	x16, .Ltally.8
	add	x16, x16, :lo12:.Ltally.8
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Ltally.8:
	.quad	.Ltally.5
	.quad	.Ltally.6
	.text
.Ltally.5:
	ldr	w0, [x29, #20]
	cmp	w0, #100
	b.gt	.Ltally.7
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Ltally.7
.Ltally.6:
	ldr	w0, [x29, #24]
	cmp	w0, #10
	b.gt	.Ltally.3
	ldr	w0, [x29, #20]
	add	w0, w0, #2
	str	w0, [x29, #20]
.Ltally.7:
	ldr	w0, [x29, #20]
	add	w0, w0, #4
	str	w0, [x29, #20]
.Ltally.3:
	ldr	w0, [x29, #24]
	add	w0, w0, #1
	str	w0, [x29, #24]
	b	.Ltally.2
.Ltally.4:
	ldr	w0, [x29, #20]
	b	.Ltally.1
.Ltally.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	tally, .-tally
	.globl	low_braced
	.type	low_braced, @function
low_braced:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
.Llow_braced.2:
	ldr	w0, [x29, #16]
	cbz	w0, .Llow_braced.3
	ldr	w0, [x29, #16]
	and	w0, w0, #0x1
	cbnz	w0, .Llow_braced.3
	ldr	w0, [x29, #16]
	asr	w0, w0, #1
	str	w0, [x29, #16]
	b	.Llow_braced.2
.Llow_braced.3:
	ldr	w0, [x29, #16]
	b	.Llow_braced.1
.Llow_braced.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	low_braced, .-low_braced
	.section	.note.GNU-stack,"",@progbits
