	.file	"array-local-list.c"
	.text
	.globl	fewer
	.type	fewer, @function
fewer:
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
	mov	w0, #1
	str	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lfewer.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fewer, .-fewer
	.globl	records
	.type	records, @function
records:
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
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lrecords.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	records, .-records
	.globl	counted
	.type	counted, @function
counted:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	xzr, [x29, #16]
	str	xzr, [x29, #24]
	mov	x0, #1
	str	x0, [x29, #16]
	mov	x0, #2
	str	x0, [x29, #24]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lcounted.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	counted, .-counted
	.globl	designated
	.type	designated, @function
designated:
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
	bl	f
	str	w0, [x29, #28]
	bl	g
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Ldesignated.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	designated, .-designated
	.globl	bytes
	.type	bytes, @function
bytes:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	wzr, [x29, #16]
	strb	wzr, [x29, #17]
	strb	wzr, [x29, #18]
	strb	wzr, [x29, #19]
	strb	wzr, [x29, #20]
	mov	w0, #97
	strb	w0, [x29, #16]
	mov	w0, #98
	strb	w0, [x29, #17]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lbytes.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	bytes, .-bytes
	.globl	large
	.type	large, @function
large:
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
.Llarge.2:
	str	xzr, [x0], #8
	subs	x16, x16, #1
	b.ne	.Llarge.2
	mov	x0, #1
	str	x0, [x29, #16]
	add	x0, x29, #16
	str	x0, [x29, #216]
	ldr	x0, [x29, #216]
	bl	use
.Llarge.1:
	.cfi_def_cfa wsp, 224
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #224
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	large, .-large
	.globl	byte_loop
	.type	byte_loop, @function
byte_loop:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	mov	x16, #20
.Lbyte_loop.2:
	strb	wzr, [x0], #1
	subs	x16, x16, #1
	b.ne	.Lbyte_loop.2
	mov	w0, #120
	strb	w0, [x29, #16]
	add	x0, x29, #16
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	use
.Lbyte_loop.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	byte_loop, .-byte_loop
	.section	.note.GNU-stack,"",@progbits
