	.file	"struct-local-list-initializer.c"
	.text
	.globl	full
	.type	full, @function
full:
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
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lfull.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	full, .-full
	.globl	nested
	.type	nested, @function
nested:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
	str	wzr, [x29, #28]
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #24]
	mov	w0, #4
	str	w0, [x29, #28]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lnested.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	arrays
	.type	arrays, @function
arrays:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
	str	wzr, [x29, #28]
	mov	w0, #97
	strb	w0, [x29, #16]
	mov	w0, #1
	str	w0, [x29, #20]
	mov	w0, #2
	str	w0, [x29, #24]
	str	wzr, [x29, #32]
	str	wzr, [x29, #36]
	str	wzr, [x29, #40]
	str	wzr, [x29, #44]
	mov	w0, #98
	strb	w0, [x29, #32]
	mov	w0, #3
	str	w0, [x29, #36]
	mov	w0, #4
	str	w0, [x29, #40]
	mov	w0, #5
	str	w0, [x29, #44]
	add	x0, x29, #16
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	use
	add	x0, x29, #32
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	bl	use
.Larrays.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	arrays, .-arrays
	.globl	first
	.type	first, @function
first:
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
	mov	w0, #120
	strb	w0, [x29, #16]
	mov	w0, #121
	strb	w0, [x29, #17]
	mov	w0, #122
	strb	w0, [x29, #18]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lfirst.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	first, .-first
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
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	bl	f
	str	w0, [x29, #16]
	bl	g
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
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
	.globl	counted
	.type	counted, @function
counted:
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
	str	wzr, [x29, #24]
	str	wzr, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #24]
.Lcounted.2:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lcounted.4
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	add	w1, w1, w0
	str	w1, [x29, #16]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lcounted.2
.Lcounted.4:
	ldr	w0, [x29, #16]
	b	.Lcounted.1
.Lcounted.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	counted, .-counted
	.section	.note.GNU-stack,"",@progbits
