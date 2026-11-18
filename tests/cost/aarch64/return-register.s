	.file	"return-register.c"
	.text
	.globl	copied
	.type	copied, @function
copied:
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
	str	w2, [x29, #36]
	ldr	w19, [x29, #36]
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	ldr	w0, [x0, w1, sxtw #2]
	sub	w0, w19, w0
	b	.Lcopied.1
.Lcopied.1:
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
	.size	copied, .-copied
	.globl	extended
	.type	extended, @function
extended:
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
	ldr	w1, [x29, #20]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	sxtw	x0, w0
	b	.Lextended.1
.Lextended.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	extended, .-extended
	.globl	reloaded
	.type	reloaded, @function
reloaded:
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
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	cbz	w0, .Lreloaded.2
	mov	w0, #1
	b	.Lreloaded.3
.Lreloaded.2:
	mov	w0, #2
.Lreloaded.3:
	ldr	x1, [x29, #24]
	add	w0, w1, w0
	b	.Lreloaded.1
.Lreloaded.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	reloaded, .-reloaded
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
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	cmp	w0, w1
	cset	w0, lt
	b	.Lcompared.1
.Lcompared.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compared, .-compared
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
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	sdiv	w2, w0, w1
	msub	w0, w2, w1, w0
	cmp	w0, #0
	cset	w0, eq
	b	.Lnegated.1
.Lnegated.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	negated, .-negated
	.globl	stored
	.type	stored, @function
stored:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	x2, [x29, #32]
	ldr	x1, [x29, #16]
	ldr	w2, [x29, #24]
	ldr	x0, [x29, #32]
	str	x0, [x1, w2, sxtw #3]
	b	.Lstored.1
.Lstored.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	added
	.type	added, @function
added:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	x2, [x29, #32]
	ldr	x1, [x29, #16]
	ldr	w2, [x29, #24]
	ldr	x0, [x29, #32]
	ldr	x3, [x1, w2, sxtw #3]
	add	x0, x3, x0
	str	x0, [x1, w2, sxtw #3]
	b	.Ladded.1
.Ladded.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	added, .-added
	.globl	stepped
	.type	stepped, @function
stepped:
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
	add	w1, w0, #1
	str	w1, [x29, #16]
	b	.Lstepped.1
.Lstepped.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	stepped_element
	.type	stepped_element, @function
stepped_element:
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
	ldr	x1, [x29, #16]
	ldr	w2, [x29, #24]
	ldr	w0, [x1, w2, sxtw #2]
	add	w3, w0, #1
	str	w3, [x1, w2, sxtw #2]
	b	.Lstepped_element.1
.Lstepped_element.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stepped_element, .-stepped_element
	.globl	divided
	.type	divided, @function
divided:
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
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #28]
	sdiv	w0, w0, w1
	b	.Ldivided.1
.Ldivided.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	divided, .-divided
	.globl	multiplied
	.type	multiplied, @function
multiplied:
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
	mov	w0, #4
	mul	w0, w19, w0
	b	.Lmultiplied.1
.Lmultiplied.1:
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
	.size	multiplied, .-multiplied
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
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	str	w0, [x29, #24]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #24]
	str	w0, [x29, #28]
	ldr	w0, [x29, #28]
	bl	f
	add	w0, w19, w0
	b	.Lcalled.1
.Lcalled.1:
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
	.size	called, .-called
	.globl	deeper
	.type	deeper, @function
deeper:
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
	str	w2, [x29, #36]
	ldr	w19, [x29, #36]
	ldr	x0, [x29, #24]
	ldr	w1, [x29, #32]
	mov	w2, #2
	sdiv	w1, w1, w2
	ldr	w0, [x0, w1, sxtw #2]
	sub	w0, w19, w0
	b	.Ldeeper.1
.Ldeeper.1:
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
	.size	deeper, .-deeper
	.globl	written
	.type	written, @function
written:
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
	add	w19, w19, #2
	add	w0, w0, w19
	b	.Lwritten.1
.Lwritten.1:
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
	.size	written, .-written
	.globl	remainder_reloaded
	.type	remainder_reloaded, @function
remainder_reloaded:
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
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	cbz	w0, .Lremainder_reloaded.2
	mov	w0, #3
	b	.Lremainder_reloaded.3
.Lremainder_reloaded.2:
	mov	w0, #5
.Lremainder_reloaded.3:
	ldr	x1, [x29, #24]
	sdiv	w2, w1, w0
	msub	w0, w2, w0, w1
	sxtw	x0, w0
	b	.Lremainder_reloaded.1
.Lremainder_reloaded.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	remainder_reloaded, .-remainder_reloaded
	.globl	compared_reloaded
	.type	compared_reloaded, @function
compared_reloaded:
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
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	cbz	w0, .Lcompared_reloaded.2
	mov	w0, #1
	b	.Lcompared_reloaded.3
.Lcompared_reloaded.2:
	mov	w0, #2
.Lcompared_reloaded.3:
	ldr	x1, [x29, #24]
	cmp	w1, w0
	cset	w0, lt
	b	.Lcompared_reloaded.1
.Lcompared_reloaded.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compared_reloaded, .-compared_reloaded
	.globl	negated_reloaded
	.type	negated_reloaded, @function
negated_reloaded:
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
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	cbz	w0, .Lnegated_reloaded.2
	mov	w0, #1
	b	.Lnegated_reloaded.3
.Lnegated_reloaded.2:
	mov	w0, #2
.Lnegated_reloaded.3:
	ldr	x1, [x29, #24]
	add	w1, w1, w0
	neg	w0, w1
	b	.Lnegated_reloaded.1
.Lnegated_reloaded.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	negated_reloaded, .-negated_reloaded
	.globl	difference_reloaded
	.type	difference_reloaded, @function
difference_reloaded:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	w3, [x29, #40]
	ldr	x0, [x29, #16]
	str	x0, [x29, #48]
	ldr	w0, [x29, #40]
	cbz	w0, .Ldifference_reloaded.2
	ldr	x0, [x29, #24]
	b	.Ldifference_reloaded.3
.Ldifference_reloaded.2:
	ldr	x0, [x29, #32]
.Ldifference_reloaded.3:
	ldr	x1, [x29, #48]
	sub	x1, x1, x0
	mov	x0, #4
	sdiv	x0, x1, x0
	b	.Ldifference_reloaded.1
.Ldifference_reloaded.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	difference_reloaded, .-difference_reloaded
	.globl	indexed_reloaded
	.type	indexed_reloaded, @function
indexed_reloaded:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	w2, [x29, #28]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #28]
	cbz	w0, .Lindexed_reloaded.2
	ldr	w0, [x29, #24]
	b	.Lindexed_reloaded.3
.Lindexed_reloaded.2:
	mov	w0, #0
.Lindexed_reloaded.3:
	ldr	x1, [x29, #32]
	add	x0, x1, w0, sxtw #2
	b	.Lindexed_reloaded.1
.Lindexed_reloaded.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	indexed_reloaded, .-indexed_reloaded
	.globl	assigned_reloaded
	.type	assigned_reloaded, @function
assigned_reloaded:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #20]
	cbz	w0, .Lassigned_reloaded.2
	mov	w0, #1
	b	.Lassigned_reloaded.3
.Lassigned_reloaded.2:
	mov	w0, #2
.Lassigned_reloaded.3:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	str	w0, [x29, #24]
	b	.Lassigned_reloaded.1
.Lassigned_reloaded.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	assigned_reloaded, .-assigned_reloaded
	.globl	stored_widened
	.type	stored_widened, @function
stored_widened:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x1, [x29, #16]
	ldr	x0, [x29, #24]
	str	w0, [x1]
	mov	w0, w0
	b	.Lstored_widened.1
.Lstored_widened.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_widened, .-stored_widened
	.globl	stored_held
	.type	stored_held, @function
stored_held:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	w2, [x29, #28]
	str	w3, [x29, #32]
	ldr	x1, [x29, #16]
	ldr	w2, [x29, #24]
	ldr	w0, [x29, #28]
	ldr	w3, [x29, #32]
	sub	w0, w0, w3
	str	w0, [x1, w2, sxtw #2]
	b	.Lstored_held.1
.Lstored_held.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_held, .-stored_held
	.globl	incremented_element
	.type	incremented_element, @function
incremented_element:
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
	ldr	x1, [x29, #16]
	ldr	w2, [x29, #24]
	ldr	w0, [x1, w2, sxtw #2]
	add	w0, w0, #1
	str	w0, [x1, w2, sxtw #2]
	b	.Lincremented_element.1
.Lincremented_element.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	incremented_element, .-incremented_element
	.globl	compound_reloaded
	.type	compound_reloaded, @function
compound_reloaded:
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
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	cbz	w0, .Lcompound_reloaded.2
	mov	w0, #1
	b	.Lcompound_reloaded.3
.Lcompound_reloaded.2:
	mov	w0, #2
.Lcompound_reloaded.3:
	ldr	w1, [x29, #24]
	add	w0, w1, w0
	str	w0, [x29, #24]
	b	.Lcompound_reloaded.1
.Lcompound_reloaded.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compound_reloaded, .-compound_reloaded
	.globl	called_place
	.type	called_place, @function
called_place:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	str	w2, [x29, #28]
	ldr	x1, [x29, #16]
	str	x1, [x29, #32]
	ldr	w1, [x29, #24]
	str	w1, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	ldr	w1, [x29, #28]
	ldr	x2, [x29, #32]
	str	w1, [x2, w0, sxtw #2]
	mov	w0, w1
	b	.Lcalled_place.1
.Lcalled_place.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	called_place, .-called_place
	.globl	written_branched
	.type	written_branched, @function
written_branched:
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
	ldr	w19, [x29, #24]
	str	x19, [x29, #32]
	ldr	w0, [x29, #28]
	cbz	w0, .Lwritten_branched.2
	mov	w0, w19
	add	w19, w19, #1
	b	.Lwritten_branched.3
.Lwritten_branched.2:
	mov	w0, #2
.Lwritten_branched.3:
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	b	.Lwritten_branched.1
.Lwritten_branched.1:
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
	.size	written_branched, .-written_branched
	.globl	written_nested
	.type	written_nested, @function
written_nested:
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
	mov	w1, w19
	mov	w19, #2
	add	w0, w0, w19
	add	w0, w1, w0
	b	.Lwritten_nested.1
.Lwritten_nested.1:
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
	.size	written_nested, .-written_nested
	.globl	written_called
	.type	written_called, @function
written_called:
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
	ldr	w19, [x29, #24]
	mov	w0, w19
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	mov	w19, w0
	ldr	x1, [x29, #32]
	add	w0, w1, w0
	b	.Lwritten_called.1
.Lwritten_called.1:
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
	.size	written_called, .-written_called
	.globl	written_sum
	.type	written_sum, @function
written_sum:
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
	str	w2, [x29, #40]
	ldr	x19, [x29, #24]
	ldr	w0, [x29, #40]
	mov	x1, x19
	ldr	x19, [x29, #32]
	mov	w2, #0
	add	w0, w0, w2
	add	x0, x1, w0, sxtw
	b	.Lwritten_sum.1
.Lwritten_sum.1:
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
	.size	written_sum, .-written_sum
	.globl	written_row
	.type	written_row, @function
written_row:
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
	str	w2, [x29, #40]
	ldr	x19, [x29, #24]
	ldr	w0, [x29, #40]
	mov	x1, x19
	ldr	x19, [x29, #32]
	mov	w2, #0
	add	w0, w0, w2
	sxtw	x0, w0
	mov	x2, #12
	mul	x0, x0, x2
	add	x0, x1, x0
	b	.Lwritten_row.1
.Lwritten_row.1:
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
	.size	written_row, .-written_row
	.globl	written_difference
	.type	written_difference, @function
written_difference:
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
	str	x2, [x29, #40]
	ldr	x19, [x29, #24]
	ldr	x0, [x29, #40]
	mov	x1, x19
	ldr	x19, [x29, #32]
	mov	w2, #0
	add	x0, x0, w2, sxtw
	sub	x0, x1, x0
	b	.Lwritten_difference.1
.Lwritten_difference.1:
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
	.size	written_difference, .-written_difference
	.globl	written_quotient
	.type	written_quotient, @function
written_quotient:
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
	mov	x0, x19
	ldr	x19, [x29, #32]
	sub	x0, x0, x19
	mov	x1, #4
	sdiv	x0, x0, x1
	b	.Lwritten_quotient.1
.Lwritten_quotient.1:
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
	.size	written_quotient, .-written_quotient
	.globl	written_sum_called
	.type	written_sum_called, @function
written_sum_called:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -64
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	str	w2, [x29, #40]
	ldr	x19, [x29, #24]
	ldr	w0, [x29, #40]
	mov	x1, x19
	ldr	x19, [x29, #32]
	str	x0, [x29, #48]
	str	x1, [x29, #56]
	ldr	w0, [x29, #40]
	str	w0, [x29, #64]
	ldr	w0, [x29, #64]
	bl	f
	ldr	x1, [x29, #48]
	add	w1, w1, w0
	ldr	x0, [x29, #56]
	add	x0, x0, w1, sxtw
	b	.Lwritten_sum_called.1
.Lwritten_sum_called.1:
	.cfi_def_cfa wsp, 80
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	written_sum_called, .-written_sum_called
	.globl	written_quotient_called
	.type	written_quotient_called, @function
written_quotient_called:
	.cfi_startproc
	sub	sp, sp, #80
	.cfi_def_cfa_offset 80
	stp	x29, x30, [sp]
	.cfi_offset w29, -80
	.cfi_offset w30, -72
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -64
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	str	w2, [x29, #40]
	ldr	x19, [x29, #24]
	mov	x0, x19
	ldr	x19, [x29, #32]
	ldr	x1, [x29, #32]
	str	x0, [x29, #48]
	str	x1, [x29, #56]
	ldr	w0, [x29, #40]
	str	w0, [x29, #64]
	ldr	w0, [x29, #64]
	bl	f
	ldr	x1, [x29, #56]
	add	x1, x1, w0, sxtw #2
	ldr	x0, [x29, #48]
	sub	x0, x0, x1
	mov	x1, #4
	sdiv	x0, x0, x1
	b	.Lwritten_quotient_called.1
.Lwritten_quotient_called.1:
	.cfi_def_cfa wsp, 80
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #80
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	written_quotient_called, .-written_quotient_called
	.globl	called_minus
	.type	called_minus, @function
called_minus:
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
	ldr	w0, [x29, #24]
	str	w0, [x29, #28]
	ldr	w0, [x29, #28]
	bl	f
	sub	w0, w19, w0
	b	.Lcalled_minus.1
.Lcalled_minus.1:
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
	.size	called_minus, .-called_minus
	.globl	compound_called
	.type	compound_called, @function
compound_called:
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
	ldr	w0, [x29, #20]
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	bl	f
	ldr	w1, [x29, #16]
	add	w0, w1, w0
	str	w0, [x29, #16]
	b	.Lcompound_called.1
.Lcompound_called.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compound_called, .-compound_called
	.globl	pointer_called
	.type	pointer_called, @function
pointer_called:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	ldr	x1, [x29, #32]
	add	x0, x1, w0, sxtw
	b	.Lpointer_called.1
.Lpointer_called.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer_called, .-pointer_called
	.globl	row_called
	.type	row_called, @function
row_called:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	sxtw	x0, w0
	ldr	x1, [x29, #32]
	mov	x2, #12
	mul	x0, x0, x2
	add	x0, x1, x0
	b	.Lrow_called.1
.Lrow_called.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row_called, .-row_called
	.globl	scaled_called
	.type	scaled_called, @function
scaled_called:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	ldr	x1, [x29, #32]
	add	x0, x1, w0, sxtw #2
	b	.Lscaled_called.1
.Lscaled_called.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	scaled_called, .-scaled_called
	.globl	called_pointer
	.type	called_pointer, @function
called_pointer:
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
	ldr	w0, [x29, #24]
	str	w0, [x29, #28]
	ldr	w0, [x29, #28]
	bl	f
	ldr	x1, [x29, #16]
	add	x0, x1, w0, sxtw
	b	.Lcalled_pointer.1
.Lcalled_pointer.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	called_pointer, .-called_pointer
	.globl	register_row_called
	.type	register_row_called, @function
register_row_called:
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
	ldr	x19, [x29, #24]
	ldr	w0, [x29, #32]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	f
	sxtw	x0, w0
	mov	x1, #12
	mul	x0, x0, x1
	add	x0, x19, x0
	b	.Lregister_row_called.1
.Lregister_row_called.1:
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
	.size	register_row_called, .-register_row_called
	.globl	spilled_add
	.type	spilled_add, @function
spilled_add:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	add	w0, w1, w0
	b	.Lspilled_add.1
.Lspilled_add.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_add, .-spilled_add
	.globl	spilled_mul
	.type	spilled_mul, @function
spilled_mul:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	mul	w0, w1, w0
	b	.Lspilled_mul.1
.Lspilled_mul.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_mul, .-spilled_mul
	.globl	spilled_and
	.type	spilled_and, @function
spilled_and:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	and	w0, w1, w0
	b	.Lspilled_and.1
.Lspilled_and.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_and, .-spilled_and
	.globl	spilled_or
	.type	spilled_or, @function
spilled_or:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	orr	w0, w1, w0
	b	.Lspilled_or.1
.Lspilled_or.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_or, .-spilled_or
	.globl	spilled_xor
	.type	spilled_xor, @function
spilled_xor:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	eor	w0, w1, w0
	b	.Lspilled_xor.1
.Lspilled_xor.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_xor, .-spilled_xor
	.globl	spilled_add_long
	.type	spilled_add_long, @function
spilled_add_long:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	lf
	ldr	x1, [x29, #32]
	add	x0, x1, x0
	b	.Lspilled_add_long.1
.Lspilled_add_long.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_add_long, .-spilled_add_long
	.globl	spilled_mul_long
	.type	spilled_mul_long, @function
spilled_mul_long:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	lf
	ldr	x1, [x29, #32]
	mul	x0, x1, x0
	b	.Lspilled_mul_long.1
.Lspilled_mul_long.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_mul_long, .-spilled_mul_long
	.globl	spilled_and_long
	.type	spilled_and_long, @function
spilled_and_long:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	lf
	ldr	x1, [x29, #32]
	and	x0, x1, x0
	b	.Lspilled_and_long.1
.Lspilled_and_long.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_and_long, .-spilled_and_long
	.globl	spilled_or_long
	.type	spilled_or_long, @function
spilled_or_long:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	lf
	ldr	x1, [x29, #32]
	orr	x0, x1, x0
	b	.Lspilled_or_long.1
.Lspilled_or_long.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_or_long, .-spilled_or_long
	.globl	spilled_xor_long
	.type	spilled_xor_long, @function
spilled_xor_long:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #24]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	lf
	ldr	x1, [x29, #32]
	eor	x0, x1, x0
	b	.Lspilled_xor_long.1
.Lspilled_xor_long.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_xor_long, .-spilled_xor_long
	.globl	spilled_stored
	.type	spilled_stored, @function
spilled_stored:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	add	w1, w1, w0
	adrp	x16, stored_sum
	str	w1, [x16, :lo12:stored_sum]
.Lspilled_stored.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_stored, .-spilled_stored
	.globl	spilled_minus
	.type	spilled_minus, @function
spilled_minus:
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
	ldr	w0, [x29, #16]
	str	x0, [x29, #24]
	ldr	w0, [x29, #20]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	ldr	x1, [x29, #24]
	sub	w0, w1, w0
	b	.Lspilled_minus.1
.Lspilled_minus.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_minus, .-spilled_minus
	.globl	spilled_count
	.type	spilled_count, @function
spilled_count:
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
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	ldr	w0, [x29, #20]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	ldr	x1, [x29, #32]
	add	w1, w1, w0
	ldr	x0, [x29, #24]
	lsl	w0, w0, w1
	b	.Lspilled_count.1
.Lspilled_count.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	spilled_count, .-spilled_count
	.bss
	.globl	stored_sum
	.type	stored_sum, @object
	.size	stored_sum, 4
	.p2align 2
stored_sum:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
