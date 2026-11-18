	.file	"member.c"
	.text
	.globl	read_ptr
	.type	read_ptr, @function
read_ptr:
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
	ldr	w0, [x0, #8]
	b	.Lread_ptr.1
.Lread_ptr.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read_ptr, .-read_ptr
	.globl	write_ptr
	.type	write_ptr, @function
write_ptr:
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
	str	w1, [x0, #8]
.Lwrite_ptr.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	write_ptr, .-write_ptr
	.globl	step_ptr
	.type	step_ptr, @function
step_ptr:
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
	ldr	w1, [x0, #8]
	add	w1, w1, #1
	str	w1, [x0, #8]
.Lstep_ptr.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step_ptr, .-step_ptr
	.globl	add_ptr
	.type	add_ptr, @function
add_ptr:
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
	ldr	w2, [x0, #8]
	add	w2, w2, w1
	str	w2, [x0, #8]
.Ladd_ptr.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add_ptr, .-add_ptr
	.globl	read_slot
	.type	read_slot, @function
read_slot:
	.cfi_startproc
	sub	sp, sp, #160
	.cfi_def_cfa_offset 160
	stp	x29, x30, [sp]
	.cfi_offset w29, -160
	.cfi_offset w30, -152
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	b	.Lread_slot.1
.Lread_slot.1:
	.cfi_def_cfa wsp, 160
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #160
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read_slot, .-read_slot
	.globl	read_label
	.type	read_label, @function
read_label:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, g+8
	ldr	w0, [x16, :lo12:g+8]
	b	.Lread_label.1
.Lread_label.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read_label, .-read_label
	.globl	nested
	.type	nested, @function
nested:
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
	ldr	w0, [x0, #20]
	b	.Lnested.1
.Lnested.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	element
	.type	element, @function
element:
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
	ldr	x0, [x29, #16]
	add	x0, x0, #32
	ldr	x1, [x29, #24]
	ldr	w0, [x0, x1, lsl #2]
	b	.Lelement.1
.Lelement.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	element_constant
	.type	element_constant, @function
element_constant:
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
	ldr	w0, [x0, #44]
	b	.Lelement_constant.1
.Lelement_constant.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_constant, .-element_constant
	.globl	element_member
	.type	element_member, @function
element_member:
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
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #68]
	b	.Lelement_member.1
.Lelement_member.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_member, .-element_member
	.globl	element_constant_member
	.type	element_constant_member, @function
element_constant_member:
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
	ldrsh	w0, [x0, #98]
	b	.Lelement_constant_member.1
.Lelement_constant_member.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_constant_member, .-element_constant_member
	.globl	indexed
	.type	indexed, @function
indexed:
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
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	mov	x2, #144
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #8]
	b	.Lindexed.1
.Lindexed.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	indexed, .-indexed
	.globl	indexed_label
	.type	indexed_label, @function
indexed_label:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x0, tab+8
	ldr	x1, [x29, #16]
	mov	x2, #144
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, :lo12:tab+8]
	b	.Lindexed_label.1
.Lindexed_label.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	indexed_label, .-indexed_label
	.globl	address
	.type	address, @function
address:
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
	add	x0, x0, #8
	b	.Laddress.1
.Laddress.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	address, .-address
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	ldr	w0, [x19, #8]
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
	.globl	last
	.type	last, @function
last:
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
.Llast.2:
	ldr	x0, [x19, #128]
	cbz	x0, .Llast.3
	ldr	x19, [x19, #128]
	b	.Llast.2
.Llast.3:
	mov	x0, x19
	b	.Llast.1
.Llast.1:
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
	.size	last, .-last
	.globl	pun
	.type	pun, @function
pun:
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
	mov	w1, #4660
	strh	w1, [x0]
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	b	.Lpun.1
.Lpun.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pun, .-pun
	.globl	call
	.type	call, @function
call:
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
	ldr	x0, [x0, #136]
	str	x0, [x29, #32]
	ldr	w0, [x29, #24]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	ldr	x16, [x29, #32]
	blr	x16
	b	.Lcall.1
.Lcall.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	call, .-call
	.globl	element_address
	.type	element_address, @function
element_address:
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
	add	x0, x0, #44
	b	.Lelement_address.1
.Lelement_address.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_address, .-element_address
	.globl	element_member_address
	.type	element_member_address, @function
element_member_address:
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
	add	x0, x0, #98
	b	.Lelement_member_address.1
.Lelement_member_address.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_member_address, .-element_member_address
	.globl	element_address_slot
	.type	element_address_slot, @function
element_address_slot:
	.cfi_startproc
	sub	sp, sp, #176
	.cfi_def_cfa_offset 176
	stp	x29, x30, [sp]
	.cfi_offset w29, -176
	.cfi_offset w30, -168
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #60
	str	x0, [x29, #160]
	ldr	x0, [x29, #160]
	bl	use
.Lelement_address_slot.1:
	.cfi_def_cfa wsp, 176
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #176
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_address_slot, .-element_address_slot
	.globl	element_address_label
	.type	element_address_label, @function
element_address_label:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, g+44
	add	x0, x0, :lo12:g+44
	b	.Lelement_address_label.1
.Lelement_address_label.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_address_label, .-element_address_label
	.globl	held_member
	.type	held_member, @function
held_member:
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
	str	w2, [x29, #32]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	mov	x2, #16
	mul	x1, x1, x2
	add	x0, x0, x1
	str	x0, [x29, #40]
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #136]
	str	x0, [x29, #48]
	ldr	w0, [x29, #32]
	str	w0, [x29, #56]
	ldr	w0, [x29, #56]
	ldr	x16, [x29, #48]
	blr	x16
	ldr	x1, [x29, #40]
	str	w0, [x1, #68]
.Lheld_member.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held_member, .-held_member
	.globl	held_element
	.type	held_element, @function
held_element:
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
	str	w2, [x29, #32]
	ldr	x0, [x29, #16]
	add	x0, x0, #32
	ldr	x1, [x29, #24]
	add	x0, x0, x1, lsl #2
	str	x0, [x29, #40]
	ldr	x0, [x29, #16]
	ldr	x0, [x0, #136]
	str	x0, [x29, #48]
	ldr	w0, [x29, #32]
	str	w0, [x29, #56]
	ldr	w0, [x29, #56]
	ldr	x16, [x29, #48]
	blr	x16
	ldr	x2, [x29, #40]
	ldr	w1, [x2]
	add	w1, w1, w0
	str	w1, [x2]
.Lheld_element.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held_element, .-held_element
	.globl	element_indexed
	.type	element_indexed, @function
element_indexed:
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
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	mov	x2, #144
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #44]
	b	.Lelement_indexed.1
.Lelement_indexed.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_indexed, .-element_indexed
	.globl	element_indexed_label
	.type	element_indexed_label, @function
element_indexed_label:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x0, tab+44
	ldr	x1, [x29, #16]
	mov	x2, #144
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, :lo12:tab+44]
	b	.Lelement_indexed_label.1
.Lelement_indexed_label.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element_indexed_label, .-element_indexed_label
	.globl	indexed_label_odd
	.type	indexed_label_odd, @function
indexed_label_odd:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x0, tab
	add	x0, x0, :lo12:tab
	ldr	x1, [x29, #16]
	mov	x2, #144
	mul	x1, x1, x2
	add	x0, x0, x1
	ldr	w0, [x0, #2]
	b	.Lindexed_label_odd.1
.Lindexed_label_odd.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	indexed_label_odd, .-indexed_label_odd
	.bss
	.globl	g
	.type	g, @object
	.size	g, 144
	.p2align 3
g:
	.zero	144
	.globl	tab
	.type	tab, @object
	.size	tab, 1440
	.p2align 3
tab:
	.zero	1440
	.section	.note.GNU-stack,"",@progbits
