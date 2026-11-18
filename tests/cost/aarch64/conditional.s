	.file	"conditional.c"
	.text
	.globl	conditional
	.type	conditional, @function
conditional:
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
	cbz	w0, .Lconditional.2
	ldr	w0, [x29, #20]
	b	.Lconditional.3
.Lconditional.2:
	ldr	w0, [x29, #24]
.Lconditional.3:
	b	.Lconditional.1
.Lconditional.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	conditional, .-conditional
	.globl	stepped
	.type	stepped, @function
stepped:
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
	str	x2, [x29, #24]
	str	w3, [x29, #32]
	ldr	w0, [x29, #16]
	cbz	w0, .Lstepped.2
	ldr	x1, [x29, #24]
	ldr	w2, [x29, #32]
	ldr	w0, [x1, w2, sxtw #2]
	add	w3, w0, #1
	str	w3, [x1, w2, sxtw #2]
	b	.Lstepped.3
.Lstepped.2:
	ldr	w0, [x29, #20]
.Lstepped.3:
	b	.Lstepped.1
.Lstepped.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	stored
	.type	stored, @function
stored:
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
	cbz	w0, .Lstored.2
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lstored.3
.Lstored.2:
	ldr	w0, [x29, #28]
.Lstored.3:
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	summed
	.type	summed, @function
summed:
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
	str	w4, [x29, #32]
	ldr	w0, [x29, #16]
	cbz	w0, .Lsummed.2
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lsummed.3
.Lsummed.2:
	ldr	w0, [x29, #28]
.Lsummed.3:
	ldr	w1, [x29, #32]
	add	w0, w0, w1
	b	.Lsummed.1
.Lsummed.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	summed, .-summed
	.globl	widened
	.type	widened, @function
widened:
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
	cbz	w0, .Lwidened.2
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	sxtw	x0, w0
	b	.Lwidened.3
.Lwidened.2:
	ldrsw	x0, [x29, #24]
.Lwidened.3:
	b	.Lwidened.1
.Lwidened.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened, .-widened
	.globl	widened_constants
	.type	widened_constants, @function
widened_constants:
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
	cbz	w0, .Lwidened_constants.2
	mov	x0, #4
	b	.Lwidened_constants.3
.Lwidened_constants.2:
	mov	x0, #-5
.Lwidened_constants.3:
	b	.Lwidened_constants.1
.Lwidened_constants.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened_constants, .-widened_constants
	.globl	widened_quotients
	.type	widened_quotients, @function
widened_quotients:
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
	cbz	w0, .Lwidened_quotients.2
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w0, w0, w1
	b	.Lwidened_quotients.3
.Lwidened_quotients.2:
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #20]
	sdiv	w0, w0, w1
.Lwidened_quotients.3:
	sxtw	x0, w0
	b	.Lwidened_quotients.1
.Lwidened_quotients.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened_quotients, .-widened_quotients
	.globl	widened_stored
	.type	widened_stored, @function
widened_stored:
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
	cbz	w0, .Lwidened_stored.2
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	b	.Lwidened_stored.3
.Lwidened_stored.2:
	ldr	w0, [x29, #24]
.Lwidened_stored.3:
	sxtw	x0, w0
	adrp	x16, h
	str	x0, [x16, :lo12:h]
.Lwidened_stored.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened_stored, .-widened_stored
	.globl	summed_arm
	.type	summed_arm, @function
summed_arm:
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
	ldr	w0, [x29, #16]
	cbz	w0, .Lsummed_arm.2
	mov	w0, #0
	b	.Lsummed_arm.3
.Lsummed_arm.2:
	ldr	w0, [x29, #20]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	cbz	w0, .Lsummed_arm.4
	mov	w0, #1
	b	.Lsummed_arm.5
.Lsummed_arm.4:
	mov	w0, #2
.Lsummed_arm.5:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
.Lsummed_arm.3:
	b	.Lsummed_arm.1
.Lsummed_arm.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	summed_arm, .-summed_arm
	.globl	widened_step
	.type	widened_step, @function
widened_step:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	str	w2, [x29, #32]
	ldr	w0, [x29, #16]
	cbz	w0, .Lwidened_step.2
	ldr	x1, [x29, #24]
	ldr	w0, [x1]
	add	w2, w0, #1
	str	w2, [x1]
	sxtw	x0, w0
	b	.Lwidened_step.3
.Lwidened_step.2:
	ldrsw	x0, [x29, #32]
.Lwidened_step.3:
	b	.Lwidened_step.1
.Lwidened_step.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened_step, .-widened_step
	.globl	widened_chars
	.type	widened_chars, @function
widened_chars:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	strb	w2, [x29, #32]
	ldr	w0, [x29, #16]
	cbz	w0, .Lwidened_chars.2
	ldr	x1, [x29, #24]
	ldrb	w0, [x1]
	add	w2, w0, #1
	strb	w2, [x1]
	sxtw	x0, w0
	b	.Lwidened_chars.3
.Lwidened_chars.2:
	ldrb	w0, [x29, #32]
.Lwidened_chars.3:
	b	.Lwidened_chars.1
.Lwidened_chars.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened_chars, .-widened_chars
	.globl	narrowed_stored
	.type	narrowed_stored, @function
narrowed_stored:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	strb	w2, [x29, #32]
	ldr	w0, [x29, #16]
	cbz	w0, .Lnarrowed_stored.2
	ldrb	w0, [x29, #32]
	b	.Lnarrowed_stored.3
.Lnarrowed_stored.2:
	ldr	x1, [x29, #24]
	ldrb	w0, [x29, #32]
	strb	w0, [x1]
.Lnarrowed_stored.3:
	b	.Lnarrowed_stored.1
.Lnarrowed_stored.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrowed_stored, .-narrowed_stored
	.globl	widened_called
	.type	widened_called, @function
widened_called:
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
	cbz	w0, .Lwidened_called.2
	bl	first
	ldr	w1, [x29, #20]
	str	w1, [x0]
	b	.Lwidened_called.3
.Lwidened_called.2:
	bl	second
	ldr	w1, [x29, #20]
	str	w1, [x0]
.Lwidened_called.3:
	sxtw	x0, w1
	b	.Lwidened_called.1
.Lwidened_called.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	widened_called, .-widened_called
	.globl	joined_called
	.type	joined_called, @function
joined_called:
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
	cbz	w0, .Ljoined_called.2
	bl	first
	ldr	w1, [x29, #20]
	str	w1, [x0]
	mov	w0, w1
	b	.Ljoined_called.3
.Ljoined_called.2:
	bl	second
	ldr	w1, [x29, #20]
	str	w1, [x0]
	mov	w0, w1
.Ljoined_called.3:
	b	.Ljoined_called.1
.Ljoined_called.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	joined_called, .-joined_called
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	h
	.type	h, @object
	.size	h, 8
	.p2align 3
h:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
