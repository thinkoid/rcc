	.file	"operation-destination.c"
	.text
	.globl	constant_left
	.type	constant_left, @function
constant_left:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	w0, [x29, #32]
	str	w1, [x29, #36]
	ldr	w19, [x29, #32]
	mov	w20, #5
	sub	w20, w20, w19
	ldr	w0, [x29, #36]
	add	w0, w20, w0
	b	.Lconstant_left.1
.Lconstant_left.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_left, .-constant_left
	.globl	constant_wide
	.type	constant_wide, @function
constant_wide:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	w0, [x29, #32]
	str	w1, [x29, #36]
	ldr	w19, [x29, #32]
	mov	w20, #22136
	movk	w20, #0x1234, lsl #16
	sub	w20, w20, w19
	ldr	w0, [x29, #36]
	add	w0, w20, w0
	b	.Lconstant_wide.1
.Lconstant_wide.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_wide, .-constant_wide
	.globl	constant_itself
	.type	constant_itself, @function
constant_itself:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	ldr	w19, [x29, #24]
	mov	w0, #5
	sub	w19, w0, w19
	mov	w0, w19
	b	.Lconstant_itself.1
.Lconstant_itself.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_itself, .-constant_itself
	.globl	constant_shift
	.type	constant_shift, @function
constant_shift:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	w0, [x29, #32]
	str	w1, [x29, #36]
	ldr	w19, [x29, #32]
	mov	w20, #5
	lsl	w20, w20, w19
	ldr	w0, [x29, #36]
	add	w0, w20, w0
	b	.Lconstant_shift.1
.Lconstant_shift.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_shift, .-constant_shift
	.globl	retyped_twice
	.type	retyped_twice, @function
retyped_twice:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	mov	w19, w19
	ldr	w19, [x29, #24]
	add	w19, w19, #5
	ldr	w0, [x29, #28]
	add	w0, w19, w0
	b	.Lretyped_twice.1
.Lretyped_twice.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	retyped_twice, .-retyped_twice
	.globl	difference
	.type	difference, @function
difference:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -48
	.cfi_offset w20, -40
	str	x0, [x29, #32]
	str	x1, [x29, #40]
	ldr	x19, [x29, #32]
	str	wzr, [x29, #48]
	add	x0, x29, #48
	sub	x20, x19, x0
	mov	x0, #4
	sdiv	x20, x20, x0
	ldr	x0, [x29, #40]
	add	x0, x20, x0
	b	.Ldifference.1
.Ldifference.1:
	.cfi_def_cfa wsp, 64
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	difference, .-difference
	.globl	difference_char
	.type	difference_char, @function
difference_char:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -48
	.cfi_offset w20, -40
	str	x0, [x29, #32]
	str	x1, [x29, #40]
	ldr	x19, [x29, #32]
	strb	wzr, [x29, #48]
	add	x0, x29, #48
	sub	x20, x19, x0
	ldr	x0, [x29, #40]
	add	x0, x20, x0
	b	.Ldifference_char.1
.Ldifference_char.1:
	.cfi_def_cfa wsp, 64
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	difference_char, .-difference_char
	.section	.note.GNU-stack,"",@progbits
