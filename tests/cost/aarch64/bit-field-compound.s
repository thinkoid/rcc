	.file	"bit-field-compound.c"
	.text
	.globl	bump
	.type	bump, @function
bump:
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
	ldrb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsr	w1, w1, #4
	add	w1, w1, #2
	lsl	w1, w1, #4
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xffffff0f
	orr	w1, w1, w2
	strb	w1, [x0]
.Lbump.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	bump, .-bump
	.globl	raised
	.type	raised, @function
raised:
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
	ldrb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsr	w1, w1, #4
	add	w1, w1, #2
	lsl	w1, w1, #4
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xffffff0f
	orr	w1, w1, w2
	strb	w1, [x0]
	uxtb	w1, w1
	lsr	w0, w1, #4
	b	.Lraised.1
.Lraised.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	raised, .-raised
	.globl	step
	.type	step, @function
step:
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
	ldrb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsl	w1, w1, #28
	lsr	w1, w1, #28
	add	w1, w1, #1
	and	w1, w1, #0xf
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xfffffff0
	orr	w1, w1, w2
	strb	w1, [x0]
.Lstep.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step, .-step
	.globl	stepped
	.type	stepped, @function
stepped:
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
	ldrb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsl	w1, w1, #28
	lsr	w1, w1, #28
	add	w1, w1, #1
	and	w1, w1, #0xf
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xfffffff0
	orr	w1, w1, w2
	strb	w1, [x0]
.Lstepped.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	before
	.type	before, @function
before:
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
	ldrb	w0, [x0]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsl	w1, w1, #28
	lsr	w1, w1, #28
	add	w1, w1, #1
	and	w1, w1, #0xf
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xfffffff0
	orr	w1, w1, w2
	strb	w1, [x0]
	ldrb	w0, [x29, #24]
	lsl	w0, w0, #28
	lsr	w0, w0, #28
	b	.Lbefore.1
.Lbefore.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	before, .-before
	.globl	lowered
	.type	lowered, @function
lowered:
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
	ldrsb	w0, [x0, #1]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrsb	w1, [x29, #24]
	lsl	w1, w1, #27
	asr	w1, w1, #27
	sub	w1, w1, #1
	and	w1, w1, #0x1f
	ldrsb	w2, [x29, #24]
	and	w2, w2, #0xffffffe0
	orr	w1, w1, w2
	strb	w1, [x0, #1]
	sxtb	w1, w1
	lsl	w1, w1, #27
	asr	w0, w1, #27
	b	.Llowered.1
.Llowered.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	lowered, .-lowered
	.globl	toggle
	.type	toggle, @function
toggle:
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
	ldrb	w0, [x0, #1]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsl	w1, w1, #26
	lsr	w1, w1, #31
	eor	w1, w1, #0x1
	cmp	w1, #0
	cset	w1, ne
	lsl	w1, w1, #5
	and	w1, w1, #0x20
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xffffffdf
	orr	w1, w1, w2
	strb	w1, [x0, #1]
.Ltoggle.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	toggle, .-toggle
	.globl	turned
	.type	turned, @function
turned:
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
	ldrb	w0, [x0, #1]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrb	w1, [x29, #24]
	lsl	w1, w1, #26
	lsr	w1, w1, #31
	sub	w1, w1, #1
	cmp	w1, #0
	cset	w1, ne
	lsl	w1, w1, #5
	and	w1, w1, #0x20
	ldrb	w2, [x29, #24]
	and	w2, w2, #0xffffffdf
	orr	w1, w1, w2
	strb	w1, [x0, #1]
	ldrb	w0, [x29, #24]
	lsl	w0, w0, #26
	lsr	w0, w0, #31
	b	.Lturned.1
.Lturned.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	turned, .-turned
	.globl	ticked
	.type	ticked, @function
ticked:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	bl	tick
	str	w0, [x29, #28]
	ldr	x0, [x29, #16]
	ldrsb	w0, [x0, #1]
	strb	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldrsb	w1, [x29, #24]
	lsl	w1, w1, #27
	asr	w1, w1, #27
	ldr	w2, [x29, #28]
	add	w1, w1, w2
	and	w1, w1, #0x1f
	ldrsb	w2, [x29, #24]
	and	w2, w2, #0xffffffe0
	orr	w1, w1, w2
	strb	w1, [x0, #1]
.Lticked.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	ticked, .-ticked
	.section	.note.GNU-stack,"",@progbits
