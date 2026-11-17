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
	adrp	x0, defined
	add	x0, x0, :lo12:defined
	mov	x19, x0
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
	adrp	x0, :got:declared
	ldr	x0, [x0, :got_lo12:declared]
	mov	x19, x0
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
	.section	.note.GNU-stack,"",@progbits
