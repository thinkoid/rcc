	.file	"parameter-size-entry.c"
	.text
	.globl	mark
	.type	mark, @function
mark:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, order
	adrp	x16, marks
	ldr	w1, [x16, :lo12:marks]
	add	w2, w1, #1
	adrp	x16, marks
	str	w2, [x16, :lo12:marks]
	add	x0, x0, w1, sxtw #2
	ldr	w1, [x29, #16]
	str	w1, [x0, :lo12:order]
	ldr	w0, [x29, #16]
	b	.Lmark.1
.Lmark.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	mark, .-mark
	.globl	in_order
	.type	in_order, @function
in_order:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x16, marks
	ldr	w0, [x16, :lo12:marks]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.eq	.Lin_order.2
	mov	w0, #0
	b	.Lin_order.1
.Lin_order.2:
	str	wzr, [x29, #20]
.Lin_order.3:
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #16]
	cmp	w0, w1
	b.ge	.Lin_order.5
	adrp	x0, order
	ldr	w1, [x29, #20]
	add	x0, x0, w1, sxtw #2
	ldr	w0, [x0, :lo12:order]
	ldr	w1, [x29, #20]
	add	w1, w1, #1
	cmp	w0, w1
	b.eq	.Lin_order.6
	mov	w0, #0
	b	.Lin_order.1
.Lin_order.6:
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #20]
	b	.Lin_order.3
.Lin_order.5:
	adrp	x16, marks
	str	wzr, [x16, :lo12:marks]
	mov	w0, #1
	b	.Lin_order.1
.Lin_order.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_order, .-in_order
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
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
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
	.globl	qualified
	.type	qualified, @function
qualified:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	mov	w1, #10
	mul	w0, w0, w1
	ldr	x1, [x29, #24]
	ldr	w1, [x1]
	add	w0, w0, w1
	b	.Lqualified.1
.Lqualified.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	qualified, .-qualified
	.globl	parenthesized
	.type	parenthesized, @function
parenthesized:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	add	w0, w0, #2
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Lparenthesized.1
.Lparenthesized.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	parenthesized, .-parenthesized
	.globl	two
	.type	two, @function
two:
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
	mov	w0, #1
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	mark
	mov	w0, #2
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	mark
	mov	w0, #3
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	mark
	b	.Ltwo.1
.Ltwo.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	two, .-two
	.globl	in_register
	.type	in_register, @function
in_register:
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
	mov	w19, w0
	str	x1, [x29, #24]
	sub	w19, w19, #1
	mov	w0, w19
	b	.Lin_register.1
.Lin_register.1:
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
	.size	in_register, .-in_register
	.type	inline_size, @function
inline_size:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	mark
	ldr	w0, [x29, #16]
	add	w0, w0, #1
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	mark
	b	.Linline_size.1
.Linline_size.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	inline_size, .-inline_size
	.globl	constant
	.type	constant, @function
constant:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	x1, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	x1, [x29, #24]
	ldr	w1, [x1, #8]
	add	w0, w0, w1
	b	.Lconstant.1
.Lconstant.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant, .-constant
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #144
	.cfi_def_cfa_offset 144
	stp	x29, x30, [sp]
	.cfi_offset w29, -144
	.cfi_offset w30, -136
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	wzr, [x29, #16]
	str	wzr, [x29, #20]
	str	wzr, [x29, #24]
	str	wzr, [x29, #28]
	mov	w0, #7
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	mov	w0, #5
	str	w0, [x29, #24]
	str	wzr, [x29, #28]
	mov	w0, #3
	str	w0, [x29, #32]
	add	x0, x29, #16
	str	x0, [x29, #40]
	ldr	w0, [x29, #32]
	ldr	x1, [x29, #40]
	bl	stepped
	cmp	w0, #4
	b.eq	.Lmain.2
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	mov	w0, #3
	str	w0, [x29, #48]
	add	x0, x29, #16
	str	x0, [x29, #56]
	ldr	w0, [x29, #48]
	ldr	x1, [x29, #56]
	bl	qualified
	cmp	w0, #47
	b.eq	.Lmain.3
	mov	w0, #102
	b	.Lmain.1
.Lmain.3:
	mov	w0, #3
	str	w0, [x29, #64]
	add	x0, x29, #16
	str	x0, [x29, #72]
	ldr	w0, [x29, #64]
	ldr	x1, [x29, #72]
	bl	parenthesized
	cmp	w0, #5
	b.eq	.Lmain.4
	mov	w0, #103
	b	.Lmain.1
.Lmain.4:
	add	x0, x29, #16
	str	x0, [x29, #80]
	add	x0, x29, #16
	str	x0, [x29, #88]
	ldr	x0, [x29, #80]
	ldr	x1, [x29, #88]
	bl	two
	cmp	w0, #3
	b.ne	.Lmain.6
	mov	w0, #3
	str	w0, [x29, #96]
	ldr	w0, [x29, #96]
	bl	in_order
	cbnz	w0, .Lmain.5
.Lmain.6:
	mov	w0, #104
	b	.Lmain.1
.Lmain.5:
	mov	w0, #9
	str	w0, [x29, #100]
	add	x0, x29, #16
	str	x0, [x29, #104]
	ldr	w0, [x29, #100]
	ldr	x1, [x29, #104]
	bl	in_register
	cmp	w0, #8
	b.eq	.Lmain.7
	mov	w0, #105
	b	.Lmain.1
.Lmain.7:
	mov	w0, #1
	str	w0, [x29, #112]
	add	x0, x29, #16
	str	x0, [x29, #120]
	ldr	w0, [x29, #112]
	ldr	x1, [x29, #120]
	bl	inline_size
	cmp	w0, #2
	b.ne	.Lmain.9
	mov	w0, #2
	str	w0, [x29, #128]
	ldr	w0, [x29, #128]
	bl	in_order
	cbnz	w0, .Lmain.8
.Lmain.9:
	mov	w0, #106
	b	.Lmain.1
.Lmain.8:
	mov	w0, #1
	str	w0, [x29, #132]
	add	x0, x29, #16
	str	x0, [x29, #136]
	ldr	w0, [x29, #132]
	ldr	x1, [x29, #136]
	bl	constant
	cmp	w0, #6
	b.ne	.Lmain.11
	adrp	x16, marks
	ldr	w0, [x16, :lo12:marks]
	cmp	w0, #0
	b.eq	.Lmain.10
.Lmain.11:
	mov	w0, #107
	b	.Lmain.1
.Lmain.10:
	mov	w0, #42
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 144
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #144
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	order
	.type	order, @object
	.size	order, 64
	.p2align 2
order:
	.zero	64
	.globl	marks
	.type	marks, @object
	.size	marks, 4
	.p2align 2
marks:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
