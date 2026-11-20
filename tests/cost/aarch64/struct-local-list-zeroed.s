	.file	"struct-local-list-zeroed.c"
	.text
	.globl	point_fewer
	.type	point_fewer, @function
point_fewer:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	add	x0, x29, #20
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lpoint_fewer.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	point_fewer, .-point_fewer
	.globl	wide_fewer
	.type	wide_fewer, @function
wide_fewer:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	xzr, [x29, #24]
	str	xzr, [x29, #32]
	str	xzr, [x29, #40]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #16]
	add	x0, x0, #1
	str	x0, [x29, #32]
	add	x0, x29, #24
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	use
.Lwide_fewer.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	wide_fewer, .-wide_fewer
	.globl	large_fewer
	.type	large_fewer, @function
large_fewer:
	.cfi_startproc
	sub	sp, sp, #224
	.cfi_def_cfa_offset 224
	stp	x29, x30, [sp]
	.cfi_offset w29, -224
	.cfi_offset w30, -216
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	mov	x16, #25
.Llarge_fewer.2:
	str	xzr, [x0], #8
	subs	x16, x16, #1
	b.ne	.Llarge_fewer.2
	mov	x0, #1
	str	x0, [x29, #16]
	mov	x0, #2
	str	x0, [x29, #24]
	add	x0, x29, #16
	str	x0, [x29, #216]
	ldr	x0, [x29, #216]
	bl	use
.Llarge_fewer.1:
	.cfi_def_cfa wsp, 224
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #224
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	large_fewer, .-large_fewer
	.globl	padded_full
	.type	padded_full, @function
padded_full:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	str	w1, [x29, #20]
	str	wzr, [x29, #24]
	str	wzr, [x29, #28]
	str	wzr, [x29, #32]
	ldrb	w0, [x29, #16]
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #28]
	ldrb	w0, [x29, #16]
	strb	w0, [x29, #32]
	add	x0, x29, #24
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	use
.Lpadded_full.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	padded_full, .-padded_full
	.globl	point_empty
	.type	point_empty, @function
point_empty:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lpoint_empty.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	point_empty, .-point_empty
	.globl	union_shorter
	.type	union_shorter, @function
union_shorter:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strh	wzr, [x29, #16]
	strh	wzr, [x29, #18]
	mov	w0, #97
	strb	w0, [x29, #16]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lunion_shorter.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	union_shorter, .-union_shorter
	.section	.note.GNU-stack,"",@progbits
