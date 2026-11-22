	.file	"bit-field-write.c"
	.text
	.globl	set_used
	.type	set_used, @function
set_used:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w1, [x0, #7]
	orr	w1, w1, #0x8
	strb	w1, [x0, #7]
.Lset_used.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	set_used, .-set_used
	.globl	clear_changed
	.type	clear_changed, @function
clear_changed:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w1, [x0, #7]
	and	w1, w1, #0xffffffef
	strb	w1, [x0, #7]
.Lclear_changed.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	clear_changed, .-clear_changed
	.globl	clear_f
	.type	clear_f, @function
clear_f:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrsb	w1, [x0]
	and	w1, w1, #0xffffff07
	strb	w1, [x0]
.Lclear_f.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	clear_f, .-clear_f
	.globl	fill_f
	.type	fill_f, @function
fill_f:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrsb	w1, [x0]
	orr	w1, w1, #0xf8
	strb	w1, [x0]
.Lfill_f.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fill_f, .-fill_f
	.globl	clear_u
	.type	clear_u, @function
clear_u:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w1, [x0, #1]
	and	w1, w1, #0xffffff80
	strb	w1, [x0, #1]
.Lclear_u.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	clear_u, .-clear_u
	.globl	fill_u
	.type	fill_u, @function
fill_u:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w1, [x0, #1]
	orr	w1, w1, #0x7f
	strb	w1, [x0, #1]
.Lfill_u.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fill_u, .-fill_u
	.globl	five
	.type	five, @function
five:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #16]
	ldrsb	w1, [x1]
	and	w1, w1, #0xffffff07
	mov	w2, #40
	orr	w1, w1, w2
	strb	w1, [x0]
.Lfive.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	five, .-five
	.globl	store
	.type	store, @function
store:
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
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	lsl	w1, w1, #3
	and	w1, w1, #0xf8
	ldr	x2, [x29, #16]
	ldrsb	w2, [x2]
	and	w2, w2, #0xffffff07
	orr	w1, w1, w2
	strb	w1, [x0]
.Lstore.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	store, .-store
	.globl	store_bool
	.type	store_bool, @function
store_bool:
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
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	cmp	w1, #0
	cset	w1, ne
	lsl	w1, w1, #7
	and	w1, w1, #0x80
	ldr	x2, [x29, #16]
	ldrb	w2, [x2, #1]
	and	w2, w2, #0xffffff7f
	orr	w1, w1, w2
	strb	w1, [x0, #1]
.Lstore_bool.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	store_bool, .-store_bool
	.globl	stored
	.type	stored, @function
stored:
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
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	lsl	w1, w1, #3
	and	w1, w1, #0xf8
	ldr	x2, [x29, #16]
	ldrsb	w2, [x2]
	and	w2, w2, #0xffffff07
	orr	w1, w1, w2
	strb	w1, [x0]
	sxtb	w1, w1
	asr	w0, w1, #3
	b	.Lstored.1
.Lstored.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored, .-stored
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
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	add	w2, w1, #1
	str	w2, [x29, #24]
	add	x0, x0, w1, sxtw #4
	ldrb	w1, [x0, #7]
	orr	w1, w1, #0x8
	strb	w1, [x0, #7]
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
	.section	.note.GNU-stack,"",@progbits
