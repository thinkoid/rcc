	.file	"struct-local-list-designators.c"
	.text
	.globl	member
	.type	member, @function
member:
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
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lmember.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	member, .-member
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
	mov	w0, #3
	str	w0, [x29, #24]
	mov	w0, #4
	str	w0, [x29, #28]
	mov	w0, #2
	str	w0, [x29, #20]
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
	.globl	elements
	.type	elements, @function
elements:
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
	str	w0, [x29, #28]
	mov	w0, #97
	strb	w0, [x29, #16]
	mov	w0, #2
	str	w0, [x29, #20]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lelements.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	elements, .-elements
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
	str	w0, [x29, #20]
	bl	g
	str	w0, [x29, #16]
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
	.globl	overridden
	.type	overridden, @function
overridden:
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
	str	w0, [x29, #16]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Loverridden.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	overridden, .-overridden
	.globl	rebraced
	.type	rebraced, @function
rebraced:
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
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #16]
	add	x0, x29, #16
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	use
.Lrebraced.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rebraced, .-rebraced
	.globl	switched
	.type	switched, @function
switched:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	bl	f
	str	w0, [x29, #16]
	str	wzr, [x29, #16]
	mov	w0, #120
	strb	w0, [x29, #16]
	add	x0, x29, #16
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
.Lswitched.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	switched, .-switched
	.section	.note.GNU-stack,"",@progbits
