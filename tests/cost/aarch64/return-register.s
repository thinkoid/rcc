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
	.section	.note.GNU-stack,"",@progbits
