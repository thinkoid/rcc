	.file	"bit-field-condition.c"
	.text
	.globl	if_used
	.type	if_used, @function
if_used:
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
	ldrb	w0, [x0, #7]
	lsl	w0, w0, #28
	lsr	w0, w0, #31
	cbz	w0, .Lif_used.2
	bl	f
.Lif_used.2:
.Lif_used.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	if_used, .-if_used
	.globl	if_not_used
	.type	if_not_used, @function
if_not_used:
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
	ldrb	w0, [x0, #7]
	lsl	w0, w0, #28
	lsr	w0, w0, #31
	cbnz	w0, .Lif_not_used.2
	bl	f
.Lif_not_used.2:
.Lif_not_used.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	if_not_used, .-if_not_used
	.globl	if_used_zero
	.type	if_used_zero, @function
if_used_zero:
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
	ldrb	w0, [x0, #7]
	lsl	w0, w0, #28
	lsr	w0, w0, #31
	cmp	w0, #0
	b.ne	.Lif_used_zero.2
	bl	f
.Lif_used_zero.2:
.Lif_used_zero.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	if_used_zero, .-if_used_zero
	.globl	used_and_waited
	.type	used_and_waited, @function
used_and_waited:
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
	ldrb	w0, [x0, #7]
	lsl	w0, w0, #28
	lsr	w0, w0, #31
	cbz	w0, .Lused_and_waited.2
	ldr	x0, [x29, #16]
	ldrb	w0, [x0, #7]
	lsl	w0, w0, #29
	lsr	w0, w0, #31
	cbnz	w0, .Lused_and_waited.2
	mov	w0, #1
	b	.Lused_and_waited.3
.Lused_and_waited.2:
	mov	w0, #0
.Lused_and_waited.3:
	b	.Lused_and_waited.1
.Lused_and_waited.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	used_and_waited, .-used_and_waited
	.globl	if_level
	.type	if_level, @function
if_level:
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
	ldrsb	w0, [x0]
	lsl	w0, w0, #25
	asr	w0, w0, #28
	cbz	w0, .Lif_level.2
	bl	f
.Lif_level.2:
.Lif_level.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	if_level, .-if_level
	.globl	if_not_level
	.type	if_not_level, @function
if_not_level:
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
	ldrsb	w0, [x0]
	lsl	w0, w0, #25
	asr	w0, w0, #28
	cbnz	w0, .Lif_not_level.2
	bl	f
.Lif_not_level.2:
.Lif_not_level.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	if_not_level, .-if_not_level
	.globl	if_level_zero
	.type	if_level_zero, @function
if_level_zero:
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
	ldrsb	w0, [x0]
	lsl	w0, w0, #25
	asr	w0, w0, #28
	cmp	w0, #0
	b.ne	.Lif_level_zero.2
	bl	f
.Lif_level_zero.2:
.Lif_level_zero.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	if_level_zero, .-if_level_zero
	.section	.note.GNU-stack,"",@progbits
