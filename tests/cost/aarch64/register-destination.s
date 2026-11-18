	.file	"register-destination.c"
	.text
	.globl	load
	.type	load, @function
load:
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
	mov	w0, w19
	b	.Lload.1
.Lload.1:
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
	.size	load, .-load
	.globl	constant_leaf
	.type	constant_leaf, @function
constant_leaf:
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
	ldr	w0, [x29, #24]
	add	w19, w0, #5
	mov	w0, w19
	b	.Lconstant_leaf.1
.Lconstant_leaf.1:
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
	.size	constant_leaf, .-constant_leaf
	.globl	named_leaf
	.type	named_leaf, @function
named_leaf:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lnamed_leaf.1
.Lnamed_leaf.1:
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
	.size	named_leaf, .-named_leaf
	.globl	register_leaf
	.type	register_leaf, @function
register_leaf:
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
	mov	w19, w0
	str	w1, [x29, #32]
	ldr	w0, [x29, #32]
	add	w20, w0, w19
	mov	w0, w20
	b	.Lregister_leaf.1
.Lregister_leaf.1:
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
	.size	register_leaf, .-register_leaf
	.globl	pointer_leaf
	.type	pointer_leaf, @function
pointer_leaf:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	x1, [x29, #32]
	ldr	w0, [x29, #24]
	ldr	x1, [x29, #32]
	ldr	w1, [x1]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lpointer_leaf.1
.Lpointer_leaf.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_leaf, .-pointer_leaf
	.globl	promoted_leaf
	.type	promoted_leaf, @function
promoted_leaf:
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
	strb	w1, [x29, #28]
	ldr	w0, [x29, #24]
	ldrb	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lpromoted_leaf.1
.Lpromoted_leaf.1:
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
	.size	promoted_leaf, .-promoted_leaf
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
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	ldr	w2, [x29, #32]
	add	w1, w1, w2
	add	w19, w0, w1
	mov	w0, w19
	b	.Lnested.1
.Lnested.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	named
	.type	named, @function
named:
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
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #24]
	sub	w19, w0, w19
	mov	w0, w19
	b	.Lnamed.1
.Lnamed.1:
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
	.size	named, .-named
	.globl	itself
	.type	itself, @function
itself:
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
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	add	w19, w19, w0
	mov	w0, w19
	b	.Litself.1
.Litself.1:
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
	.size	itself, .-itself
	.globl	itself_left
	.type	itself_left, @function
itself_left:
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
	ldr	w19, [x29, #24]
	add	w0, w19, #1
	ldr	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Litself_left.1
.Litself_left.1:
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
	.size	itself_left, .-itself_left
	.globl	itself_assigned
	.type	itself_assigned, @function
itself_assigned:
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
	mov	w0, w19
	b	.Litself_assigned.1
.Litself_assigned.1:
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
	.size	itself_assigned, .-itself_assigned
	.globl	quotient
	.type	quotient, @function
quotient:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	sdiv	w19, w0, w1
	mov	w0, w19
	b	.Lquotient.1
.Lquotient.1:
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
	.size	quotient, .-quotient
	.globl	initializer
	.type	initializer, @function
initializer:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	mul	w19, w0, w1
	mov	w0, w19
	b	.Linitializer.1
.Linitializer.1:
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
	.size	initializer, .-initializer
	.globl	address_global
	.type	address_global, @function
address_global:
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
	adrp	x19, defined
	add	x19, x19, :lo12:defined
	mov	x0, x19
	b	.Laddress_global.1
.Laddress_global.1:
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
	.size	address_global, .-address_global
	.globl	address_declared
	.type	address_declared, @function
address_declared:
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
	adrp	x19, :got:declared
	ldr	x19, [x19, :got_lo12:declared]
	mov	x0, x19
	b	.Laddress_declared.1
.Laddress_declared.1:
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
	.size	address_declared, .-address_declared
	.globl	address_local
	.type	address_local, @function
address_local:
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
	add	x19, x29, #24
	ldr	w0, [x19]
	b	.Laddress_local.1
.Laddress_local.1:
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
	.size	address_local, .-address_local
	.globl	address_element
	.type	address_element, @function
address_element:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	add	x0, x29, #28
	ldr	w1, [x29, #24]
	add	x19, x0, w1, sxtw #2
	ldr	w0, [x19]
	b	.Laddress_element.1
.Laddress_element.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	address_element, .-address_element
	.globl	pointer_sum
	.type	pointer_sum, @function
pointer_sum:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x0, [x29, #24]
	ldr	x1, [x29, #32]
	add	x19, x0, x1, lsl #2
	mov	x0, x19
	b	.Lpointer_sum.1
.Lpointer_sum.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_sum, .-pointer_sum
	.globl	pointer_step
	.type	pointer_step, @function
pointer_step:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	add	x19, x19, #4
	mov	x0, x19
	b	.Lpointer_step.1
.Lpointer_step.1:
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
	.size	pointer_step, .-pointer_step
	.globl	pointer_back
	.type	pointer_back, @function
pointer_back:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	sub	x19, x19, x0, lsl #2
	mov	x0, x19
	b	.Lpointer_back.1
.Lpointer_back.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_back, .-pointer_back
	.globl	pointer_before
	.type	pointer_before, @function
pointer_before:
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
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	sub	x19, x0, #4
	mov	x0, x19
	b	.Lpointer_before.1
.Lpointer_before.1:
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
	.size	pointer_before, .-pointer_before
	.globl	negated
	.type	negated, @function
negated:
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
	ldr	w0, [x29, #24]
	neg	w19, w0
	mov	w0, w19
	b	.Lnegated.1
.Lnegated.1:
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
	.size	negated, .-negated
	.globl	complemented
	.type	complemented, @function
complemented:
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
	ldr	w0, [x29, #24]
	mvn	w19, w0
	mov	w0, w19
	b	.Lcomplemented.1
.Lcomplemented.1:
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
	.size	complemented, .-complemented
	.globl	negated_itself
	.type	negated_itself, @function
negated_itself:
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
	neg	w19, w19
	mov	w0, w19
	b	.Lnegated_itself.1
.Lnegated_itself.1:
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
	.size	negated_itself, .-negated_itself
	.globl	compared
	.type	compared, @function
compared:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	cmp	w0, w1
	cset	w19, lt
	mov	w0, w19
	b	.Lcompared.1
.Lcompared.1:
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
	.size	compared, .-compared
	.globl	compared_itself
	.type	compared_itself, @function
compared_itself:
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
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	cmp	w19, w0
	cset	w19, lt
	mov	w0, w19
	b	.Lcompared_itself.1
.Lcompared_itself.1:
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
	.size	compared_itself, .-compared_itself
	.globl	compared_constant
	.type	compared_constant, @function
compared_constant:
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
	ldr	w0, [x29, #24]
	cmp	w0, #5
	cset	w19, eq
	mov	w0, w19
	b	.Lcompared_constant.1
.Lcompared_constant.1:
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
	.size	compared_constant, .-compared_constant
	.globl	element_leaf
	.type	element_leaf, @function
element_leaf:
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
	ldr	w0, [x29, #24]
	adrp	x16, table+12
	ldr	w1, [x16, :lo12:table+12]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lelement_leaf.1
.Lelement_leaf.1:
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
	.size	element_leaf, .-element_leaf
	.globl	pointer_element_leaf
	.type	pointer_element_leaf, @function
pointer_element_leaf:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	x1, [x29, #32]
	ldr	w0, [x29, #24]
	ldr	x1, [x29, #32]
	ldr	w1, [x1, #12]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lpointer_element_leaf.1
.Lpointer_element_leaf.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_element_leaf, .-pointer_element_leaf
	.globl	named_right
	.type	named_right, @function
named_right:
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
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #24]
	add	w19, w0, w19
	mov	w0, w19
	b	.Lnamed_right.1
.Lnamed_right.1:
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
	.size	named_right, .-named_right
	.globl	own_element
	.type	own_element, @function
own_element:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #24]
	ldrb	w1, [x19, #1]
	add	x19, x0, x1
	mov	x0, x19
	b	.Lown_element.1
.Lown_element.1:
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
	.size	own_element, .-own_element
	.globl	step_char
	.type	step_char, @function
step_char:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	add	x19, x19, x0
	mov	x0, x19
	b	.Lstep_char.1
.Lstep_char.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step_char, .-step_char
	.globl	back_char
	.type	back_char, @function
back_char:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	sub	x19, x19, x0
	mov	x0, x19
	b	.Lback_char.1
.Lback_char.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	back_char, .-back_char
	.globl	step_int
	.type	step_int, @function
step_int:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	add	x19, x19, x0, lsl #2
	mov	x0, x19
	b	.Lstep_int.1
.Lstep_int.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step_int, .-step_int
	.globl	back_int
	.type	back_int, @function
back_int:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	sub	x19, x19, x0, lsl #2
	mov	x0, x19
	b	.Lback_int.1
.Lback_int.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	back_int, .-back_int
	.globl	step_row
	.type	step_row, @function
step_row:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	mov	x1, #12
	mul	x0, x0, x1
	add	x19, x19, x0
	mov	x0, x19
	b	.Lstep_row.1
.Lstep_row.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step_row, .-step_row
	.globl	back_row
	.type	back_row, @function
back_row:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #32]
	mov	x1, #12
	mul	x0, x0, x1
	sub	x19, x19, x0
	mov	x0, x19
	b	.Lback_row.1
.Lback_row.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	back_row, .-back_row
	.globl	defined_function
	.type	defined_function, @function
defined_function:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #0
	b	.Ldefined_function.1
.Ldefined_function.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	defined_function, .-defined_function
	.globl	function_defined
	.type	function_defined, @function
function_defined:
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
	adrp	x19, defined_function
	add	x19, x19, :lo12:defined_function
	mov	x0, x19
	b	.Lfunction_defined.1
.Lfunction_defined.1:
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
	.size	function_defined, .-function_defined
	.globl	function_declared
	.type	function_declared, @function
function_declared:
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
	adrp	x19, :got:declared_function
	ldr	x19, [x19, :got_lo12:declared_function]
	mov	x0, x19
	b	.Lfunction_declared.1
.Lfunction_declared.1:
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
	.size	function_declared, .-function_declared
	.globl	address_far
	.type	address_far, @function
address_far:
	.cfi_startproc
	sub	sp, sp, #4096
	.cfi_def_cfa_offset 4096
	sub	sp, sp, #3936
	.cfi_def_cfa_offset 8032
	stp	x29, x30, [sp]
	.cfi_offset w29, -8032
	.cfi_offset w30, -8024
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -8016
	add	x19, x29, #4096
	add	x19, x19, #1928
	add	x0, x29, #24
	sub	x0, x19, x0
	mov	x1, #4
	sdiv	x0, x0, x1
	b	.Laddress_far.1
.Laddress_far.1:
	.cfi_def_cfa wsp, 8032
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #4096
	.cfi_def_cfa_offset 3936
	add	sp, sp, #3936
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	address_far, .-address_far
	.globl	signedness_load
	.type	signedness_load, @function
signedness_load:
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
	mov	w0, w19
	b	.Lsignedness_load.1
.Lsignedness_load.1:
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
	.size	signedness_load, .-signedness_load
	.globl	signedness_char
	.type	signedness_char, @function
signedness_char:
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
	strb	w0, [x29, #24]
	ldrb	w19, [x29, #24]
	mov	w0, w19
	b	.Lsignedness_char.1
.Lsignedness_char.1:
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
	.size	signedness_char, .-signedness_char
	.globl	signedness_byte
	.type	signedness_byte, @function
signedness_byte:
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
	strb	w0, [x29, #24]
	ldrb	w19, [x29, #24]
	mov	w0, w19
	b	.Lsignedness_byte.1
.Lsignedness_byte.1:
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
	.size	signedness_byte, .-signedness_byte
	.globl	signedness_sum
	.type	signedness_sum, @function
signedness_sum:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lsignedness_sum.1
.Lsignedness_sum.1:
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
	.size	signedness_sum, .-signedness_sum
	.globl	signedness_unsigned_sum
	.type	signedness_unsigned_sum, @function
signedness_unsigned_sum:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lsignedness_unsigned_sum.1
.Lsignedness_unsigned_sum.1:
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
	.size	signedness_unsigned_sum, .-signedness_unsigned_sum
	.globl	signedness_compare
	.type	signedness_compare, @function
signedness_compare:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	cmp	w0, w1
	cset	w19, lt
	mov	w0, w19
	b	.Lsignedness_compare.1
.Lsignedness_compare.1:
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
	.size	signedness_compare, .-signedness_compare
	.globl	signedness_narrowed
	.type	signedness_narrowed, @function
signedness_narrowed:
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
	str	x0, [x29, #24]
	mov	w19, w19
	ldr	x0, [x29, #24]
	mov	w19, w0
	mov	w0, w19
	b	.Lsignedness_narrowed.1
.Lsignedness_narrowed.1:
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
	.size	signedness_narrowed, .-signedness_narrowed
	.globl	signedness_widened
	.type	signedness_widened, @function
signedness_widened:
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
	strb	w0, [x29, #24]
	ldrb	w0, [x29, #24]
	mov	w19, w0
	mov	w0, w19
	b	.Lsignedness_widened.1
.Lsignedness_widened.1:
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
	.size	signedness_widened, .-signedness_widened
	.globl	decay_sum
	.type	decay_sum, @function
decay_sum:
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
	str	x0, [x29, #24]
	adrp	x0, characters
	add	x0, x0, :lo12:characters
	ldr	x1, [x29, #24]
	add	x19, x0, x1
	mov	x0, x19
	b	.Ldecay_sum.1
.Ldecay_sum.1:
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
	.size	decay_sum, .-decay_sum
	.globl	decay_declared
	.type	decay_declared, @function
decay_declared:
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
	str	x0, [x29, #24]
	adrp	x0, :got:declared
	ldr	x0, [x0, :got_lo12:declared]
	ldr	x1, [x29, #24]
	add	x19, x0, x1, lsl #2
	mov	x0, x19
	b	.Ldecay_declared.1
.Ldecay_declared.1:
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
	.size	decay_declared, .-decay_declared
	.globl	decay_row
	.type	decay_row, @function
decay_row:
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
	str	x0, [x29, #24]
	adrp	x0, rows
	add	x0, x0, :lo12:rows
	ldr	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x19, x0, x1
	mov	x0, x19
	b	.Ldecay_row.1
.Ldecay_row.1:
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
	.size	decay_row, .-decay_row
	.globl	decay_twice
	.type	decay_twice, @function
decay_twice:
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
	str	x0, [x29, #24]
	adrp	x0, characters
	add	x0, x0, :lo12:characters
	ldr	x1, [x29, #24]
	add	x0, x0, x1
	ldr	x1, [x29, #24]
	add	x19, x0, x1
	mov	x0, x19
	b	.Ldecay_twice.1
.Ldecay_twice.1:
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
	.size	decay_twice, .-decay_twice
	.globl	decay_named
	.type	decay_named, @function
decay_named:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	adrp	x0, characters
	add	x0, x0, :lo12:characters
	ldrb	w1, [x19]
	add	x19, x0, x1
	mov	x0, x19
	b	.Ldecay_named.1
.Ldecay_named.1:
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
	.size	decay_named, .-decay_named
	.globl	own_value
	.type	own_value, @function
own_value:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	ldr	x19, [x29, #24]
	mov	x0, x19
	b	.Lown_value.1
.Lown_value.1:
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
	.size	own_value, .-own_value
	.globl	element_row
	.type	element_row, @function
element_row:
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
	str	x0, [x29, #24]
	adrp	x0, rows
	add	x0, x0, :lo12:rows
	ldr	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x19, x0, x1
	mov	x0, x19
	b	.Lelement_row.1
.Lelement_row.1:
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
	.size	element_row, .-element_row
	.globl	not_int
	.type	not_int, @function
not_int:
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
	ldr	w0, [x29, #24]
	cmp	w0, #0
	cset	w19, eq
	mov	w0, w19
	b	.Lnot_int.1
.Lnot_int.1:
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
	.size	not_int, .-not_int
	.globl	not_long
	.type	not_long, @function
not_long:
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
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	cmp	x0, #0
	cset	w19, eq
	mov	x0, x19
	b	.Lnot_long.1
.Lnot_long.1:
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
	.size	not_long, .-not_long
	.globl	not_char
	.type	not_char, @function
not_char:
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
	uxtb	w19, w19
	ldr	w0, [x29, #24]
	cmp	w0, #0
	cset	w0, eq
	uxtb	w19, w0
	mov	w0, w19
	b	.Lnot_char.1
.Lnot_char.1:
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
	.size	not_char, .-not_char
	.globl	step_value
	.type	step_value, @function
step_value:
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
	add	w0, w19, #1
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	add	w19, w19, w0
	ldr	w19, [x29, #24]
	sub	w0, w19, #1
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	add	w0, w19, w0
	b	.Lstep_value.1
.Lstep_value.1:
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
	.size	step_value, .-step_value
	.globl	step_pointer
	.type	step_pointer, @function
step_pointer:
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
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	w19, [x0]
	add	w1, w19, #1
	str	w1, [x0]
	mov	w0, w19
	b	.Lstep_pointer.1
.Lstep_pointer.1:
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
	.size	step_pointer, .-step_pointer
	.globl	step_register
	.type	step_register, @function
step_register:
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
	ldr	w19, [x29, #32]
	mov	w20, w19
	add	w19, w19, #1
	add	w0, w20, w19
	b	.Lstep_register.1
.Lstep_register.1:
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
	.size	step_register, .-step_register
	.globl	step_indexed
	.type	step_indexed, @function
step_indexed:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #32]
	ldr	x0, [x29, #24]
	ldr	x1, [x0, x19, lsl #3]
	add	x2, x1, #1
	str	x2, [x0, x19, lsl #3]
	mov	x19, x1
	mov	x0, x19
	b	.Lstep_indexed.1
.Lstep_indexed.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step_indexed, .-step_indexed
	.globl	step_widened
	.type	step_widened, @function
step_widened:
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
	ldr	w0, [x29, #24]
	add	w1, w0, #1
	str	w1, [x29, #24]
	sxtw	x19, w0
	ldrsw	x0, [x29, #24]
	add	x0, x19, x0
	b	.Lstep_widened.1
.Lstep_widened.1:
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
	.size	step_widened, .-step_widened
	.globl	mixed_sum
	.type	mixed_sum, @function
mixed_sum:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lmixed_sum.1
.Lmixed_sum.1:
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
	.size	mixed_sum, .-mixed_sum
	.globl	promoted_sum
	.type	promoted_sum, @function
promoted_sum:
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
	strb	w0, [x29, #24]
	str	w1, [x29, #28]
	mov	w19, w19
	ldrb	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lpromoted_sum.1
.Lpromoted_sum.1:
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
	.size	promoted_sum, .-promoted_sum
	.globl	narrowed_sum
	.type	narrowed_sum, @function
narrowed_sum:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	w1, [x29, #32]
	mov	w19, w19
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	add	w19, w0, w1
	mov	w0, w19
	b	.Lnarrowed_sum.1
.Lnarrowed_sum.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrowed_sum, .-narrowed_sum
	.globl	logical_and
	.type	logical_and, @function
logical_and:
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
	ldr	w0, [x29, #24]
	cbz	w0, .Llogical_and.2
	ldr	w0, [x29, #28]
	cbz	w0, .Llogical_and.2
	mov	w19, #1
	b	.Llogical_and.3
.Llogical_and.2:
	mov	w19, #0
.Llogical_and.3:
	mov	w0, w19
	b	.Llogical_and.1
.Llogical_and.1:
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
	.size	logical_and, .-logical_and
	.globl	logical_or
	.type	logical_or, @function
logical_or:
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
	ldr	w0, [x29, #24]
	cbnz	w0, .Llogical_or.4
	ldr	w0, [x29, #28]
	cbz	w0, .Llogical_or.2
.Llogical_or.4:
	mov	w19, #1
	b	.Llogical_or.3
.Llogical_or.2:
	mov	w19, #0
.Llogical_or.3:
	mov	w0, w19
	b	.Llogical_or.1
.Llogical_or.1:
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
	.size	logical_or, .-logical_or
	.globl	logical_itself
	.type	logical_itself, @function
logical_itself:
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
	ldr	w19, [x29, #24]
	cbz	w19, .Llogical_itself.2
	ldr	w0, [x29, #28]
	cbz	w0, .Llogical_itself.2
	mov	w19, #1
	b	.Llogical_itself.3
.Llogical_itself.2:
	mov	w19, #0
.Llogical_itself.3:
	mov	w0, w19
	b	.Llogical_itself.1
.Llogical_itself.1:
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
	.size	logical_itself, .-logical_itself
	.globl	chosen
	.type	chosen, @function
chosen:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen.2
	ldr	w19, [x29, #28]
	b	.Lchosen.3
.Lchosen.2:
	ldr	w19, [x29, #32]
.Lchosen.3:
	mov	w0, w19
	b	.Lchosen.1
.Lchosen.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen, .-chosen
	.globl	chosen_reading
	.type	chosen_reading, @function
chosen_reading:
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
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_reading.2
	add	w19, w19, #1
	b	.Lchosen_reading.3
.Lchosen_reading.2:
	mov	w19, #0
.Lchosen_reading.3:
	mov	w0, w19
	b	.Lchosen_reading.1
.Lchosen_reading.1:
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
	.size	chosen_reading, .-chosen_reading
	.globl	chosen_called
	.type	chosen_called, @function
chosen_called:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_called.2
	ldr	w0, [x29, #28]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	mov	w19, w0
	b	.Lchosen_called.3
.Lchosen_called.2:
	mov	w19, #0
.Lchosen_called.3:
	mov	w0, w19
	b	.Lchosen_called.1
.Lchosen_called.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen_called, .-chosen_called
	.globl	chosen_widened
	.type	chosen_widened, @function
chosen_widened:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_widened.2
	ldrsw	x19, [x29, #28]
	b	.Lchosen_widened.3
.Lchosen_widened.2:
	ldrsw	x19, [x29, #32]
.Lchosen_widened.3:
	mov	x0, x19
	b	.Lchosen_widened.1
.Lchosen_widened.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen_widened, .-chosen_widened
	.globl	chosen_stepped
	.type	chosen_stepped, @function
chosen_stepped:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	x2, [x29, #32]
	str	w3, [x29, #40]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_stepped.2
	ldr	w0, [x29, #28]
	str	w0, [x29, #44]
	ldr	w0, [x29, #44]
	bl	f
	mov	w19, w0
	b	.Lchosen_stepped.3
.Lchosen_stepped.2:
	ldr	x0, [x29, #32]
	ldr	w1, [x29, #40]
	ldr	w19, [x0, w1, sxtw #2]
	add	w2, w19, #1
	str	w2, [x0, w1, sxtw #2]
.Lchosen_stepped.3:
	mov	w0, w19
	b	.Lchosen_stepped.1
.Lchosen_stepped.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen_stepped, .-chosen_stepped
	.globl	chosen_divided
	.type	chosen_divided, @function
chosen_divided:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	str	w3, [x29, #36]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_divided.2
	ldr	w0, [x29, #28]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	mov	w19, w0
	b	.Lchosen_divided.3
.Lchosen_divided.2:
	ldr	w0, [x29, #32]
	ldr	w1, [x29, #36]
	sdiv	w19, w0, w1
.Lchosen_divided.3:
	mov	w0, w19
	b	.Lchosen_divided.1
.Lchosen_divided.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen_divided, .-chosen_divided
	.globl	chosen_constant
	.type	chosen_constant, @function
chosen_constant:
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
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_constant.2
	ldrsw	x19, [x29, #28]
	b	.Lchosen_constant.3
.Lchosen_constant.2:
	mov	x19, #-1
.Lchosen_constant.3:
	mov	x0, x19
	b	.Lchosen_constant.1
.Lchosen_constant.1:
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
	.size	chosen_constant, .-chosen_constant
	.globl	chosen_promoted
	.type	chosen_promoted, @function
chosen_promoted:
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
	strb	w1, [x29, #28]
	strb	w2, [x29, #29]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_promoted.2
	ldrb	w0, [x29, #28]
	b	.Lchosen_promoted.3
.Lchosen_promoted.2:
	ldrb	w0, [x29, #29]
.Lchosen_promoted.3:
	sxtw	x19, w0
	mov	x0, x19
	b	.Lchosen_promoted.1
.Lchosen_promoted.1:
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
	.size	chosen_promoted, .-chosen_promoted
	.globl	chosen_calls
	.type	chosen_calls, @function
chosen_calls:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_calls.2
	ldr	w0, [x29, #28]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	b	.Lchosen_calls.3
.Lchosen_calls.2:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	f
.Lchosen_calls.3:
	mov	w19, w0
	mov	w0, w19
	b	.Lchosen_calls.1
.Lchosen_calls.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen_calls, .-chosen_calls
	.globl	chosen_own_first
	.type	chosen_own_first, @function
chosen_own_first:
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
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_own_first.2
	b	.Lchosen_own_first.3
.Lchosen_own_first.2:
	ldr	w19, [x29, #28]
.Lchosen_own_first.3:
	mov	w0, w19
	b	.Lchosen_own_first.1
.Lchosen_own_first.1:
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
	.size	chosen_own_first, .-chosen_own_first
	.globl	chosen_own_second
	.type	chosen_own_second, @function
chosen_own_second:
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
	ldr	w19, [x29, #28]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_own_second.2
	ldr	w19, [x29, #28]
	b	.Lchosen_own_second.3
.Lchosen_own_second.2:
.Lchosen_own_second.3:
	mov	w0, w19
	b	.Lchosen_own_second.1
.Lchosen_own_second.1:
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
	.size	chosen_own_second, .-chosen_own_second
	.globl	chosen_own_pointer
	.type	chosen_own_pointer, @function
chosen_own_pointer:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	x1, [x29, #32]
	ldr	x19, [x29, #32]
	ldr	w0, [x29, #24]
	cbz	w0, .Lchosen_own_pointer.2
	ldr	x19, [x29, #32]
	b	.Lchosen_own_pointer.3
.Lchosen_own_pointer.2:
.Lchosen_own_pointer.3:
	mov	x0, x19
	b	.Lchosen_own_pointer.1
.Lchosen_own_pointer.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chosen_own_pointer, .-chosen_own_pointer
	.globl	char_operand
	.type	char_operand, @function
char_operand:
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
	strb	w0, [x29, #32]
	ldrb	w19, [x29, #32]
	add	w20, w19, #5
	mov	w0, w20
	b	.Lchar_operand.1
.Lchar_operand.1:
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
	.size	char_operand, .-char_operand
	.globl	char_operand_left
	.type	char_operand_left, @function
char_operand_left:
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
	strb	w0, [x29, #32]
	ldrb	w19, [x29, #32]
	add	w20, w19, #5
	mov	w0, w20
	b	.Lchar_operand_left.1
.Lchar_operand_left.1:
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
	.size	char_operand_left, .-char_operand_left
	.globl	int_widened
	.type	int_widened, @function
int_widened:
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
	ldr	w19, [x29, #32]
	sxtw	x0, w19
	add	x20, x0, #5
	mov	x0, x20
	b	.Lint_widened.1
.Lint_widened.1:
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
	.size	int_widened, .-int_widened
	.globl	register_product
	.type	register_product, @function
register_product:
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
	ldr	w19, [x29, #32]
	mov	w0, #4
	mul	w20, w19, w0
	add	w0, w20, w19
	b	.Lregister_product.1
.Lregister_product.1:
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
	.size	register_product, .-register_product
	.globl	prefix_step
	.type	prefix_step, @function
prefix_step:
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
	ldr	w0, [x29, #24]
	add	w19, w0, #1
	str	w19, [x29, #24]
	ldr	w0, [x29, #24]
	add	w19, w19, w0
	ldr	w0, [x29, #24]
	sub	w19, w0, #1
	str	w19, [x29, #24]
	ldr	w0, [x29, #24]
	add	w0, w19, w0
	b	.Lprefix_step.1
.Lprefix_step.1:
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
	.size	prefix_step, .-prefix_step
	.globl	prefix_pointer
	.type	prefix_pointer, @function
prefix_pointer:
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
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	add	x19, x0, #4
	str	x19, [x29, #24]
	mov	x0, x19
	b	.Lprefix_pointer.1
.Lprefix_pointer.1:
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
	.size	prefix_pointer, .-prefix_pointer
	.globl	prefix_char
	.type	prefix_char, @function
prefix_char:
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
	strb	w0, [x29, #24]
	ldrb	w0, [x29, #24]
	add	w0, w0, #1
	strb	w0, [x29, #24]
	uxtb	w19, w0
	mov	w0, w19
	b	.Lprefix_char.1
.Lprefix_char.1:
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
	.size	prefix_char, .-prefix_char
	.globl	row_element
	.type	row_element, @function
row_element:
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
	str	x0, [x29, #24]
	adrp	x0, rows
	add	x0, x0, :lo12:rows
	ldr	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x19, x0, x1
	mov	x0, x19
	b	.Lrow_element.1
.Lrow_element.1:
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
	.size	row_element, .-row_element
	.globl	row_decay
	.type	row_decay, @function
row_decay:
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
	str	x0, [x29, #24]
	adrp	x0, rows
	add	x0, x0, :lo12:rows
	ldr	x1, [x29, #24]
	mov	x2, #12
	mul	x1, x1, x2
	add	x19, x0, x1
	mov	x0, x19
	b	.Lrow_decay.1
.Lrow_decay.1:
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
	.size	row_decay, .-row_decay
	.globl	widened_sum
	.type	widened_sum, @function
widened_sum:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	add	w0, w0, w1
	sxtw	x19, w0
	mov	x0, x19
	b	.Lwidened_sum.1
.Lwidened_sum.1:
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
	.size	widened_sum, .-widened_sum
	.globl	widened_register
	.type	widened_register, @function
widened_register:
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
	ldr	w19, [x29, #32]
	sxtw	x20, w19
	sxtw	x0, w19
	add	x0, x20, x0
	b	.Lwidened_register.1
.Lwidened_register.1:
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
	.size	widened_register, .-widened_register
	.globl	widened_product
	.type	widened_product, @function
widened_product:
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
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	mul	w19, w0, w1
	mov	x0, x19
	b	.Lwidened_product.1
.Lwidened_product.1:
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
	.size	widened_product, .-widened_product
	.globl	signedness_left
	.type	signedness_left, @function
signedness_left:
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
	mov	w19, w19
	ldr	w0, [x29, #24]
	add	w19, w0, #5
	mov	w0, w19
	b	.Lsignedness_left.1
.Lsignedness_left.1:
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
	.size	signedness_left, .-signedness_left
	.bss
	.globl	defined
	.type	defined, @object
	.size	defined, 4
	.p2align 2
defined:
	.zero	4
	.globl	table
	.type	table, @object
	.size	table, 32
	.p2align 2
table:
	.zero	32
	.globl	characters
	.type	characters, @object
	.size	characters, 16
characters:
	.zero	16
	.globl	rows
	.type	rows, @object
	.size	rows, 48
	.p2align 2
rows:
	.zero	48
	.section	.note.GNU-stack,"",@progbits
