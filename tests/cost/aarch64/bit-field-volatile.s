	.file	"bit-field-volatile.c"
	.text
	.globl	status
	.type	status, @function
status:
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
	lsl	w0, w0, #31
	lsr	w0, w0, #31
	ldr	x1, [x29, #16]
	ldrb	w1, [x1, #1]
	add	w0, w0, w1
	b	.Lstatus.1
.Lstatus.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	status, .-status
	.globl	arm
	.type	arm, @function
arm:
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
	ldrb	w1, [x0]
	orr	w1, w1, #0x1
	strb	w1, [x0]
.Larm.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	arm, .-arm
	.globl	reset
	.type	reset, @function
reset:
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
	strb	wzr, [x0, #1]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	lsl	w1, w1, #1
	and	w1, w1, #0x3e
	ldr	x2, [x29, #16]
	ldrb	w2, [x2]
	and	w2, w2, #0xffffffc1
	orr	w1, w1, w2
	strb	w1, [x0]
.Lreset.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	reset, .-reset
	.section	.note.GNU-stack,"",@progbits
