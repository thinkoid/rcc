	.file	"compound.c"
	.text
	.globl	compound
	.type	compound, @function
compound:
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
	ldr	w1, [x29, #24]
	add	w1, w1, w0
	str	w1, [x29, #24]
	ldr	w0, [x29, #24]
	b	.Lcompound.1
.Lcompound.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compound, .-compound
	.globl	narrow
	.type	narrow, @function
narrow:
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
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	add	w1, w1, w0
	strb	w1, [x29, #24]
	ldrb	w0, [x29, #24]
	b	.Lnarrow.1
.Lnarrow.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	in_value
	.type	in_value, @function
in_value:
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
	strb	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldrb	w1, [x29, #24]
	add	w0, w1, w0
	strb	w0, [x29, #24]
	uxtb	w0, w0
	b	.Lin_value.1
.Lin_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_value, .-in_value
	.globl	unsigned_value
	.type	unsigned_value, @function
unsigned_value:
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
	ldr	w0, [x29, #24]
	ldr	w2, [x1]
	add	w0, w2, w0
	str	w0, [x1]
	b	.Lunsigned_value.1
.Lunsigned_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_value, .-unsigned_value
	.globl	unsigned_divided
	.type	unsigned_divided, @function
unsigned_divided:
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
	ldr	w0, [x29, #24]
	ldr	w2, [x1]
	udiv	w0, w2, w0
	str	w0, [x1]
	b	.Lunsigned_divided.1
.Lunsigned_divided.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_divided, .-unsigned_divided
	.globl	unsigned_shifted
	.type	unsigned_shifted, @function
unsigned_shifted:
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
	ldr	w0, [x29, #24]
	ldr	w2, [x1]
	lsr	w0, w2, w0
	str	w0, [x1]
	b	.Lunsigned_shifted.1
.Lunsigned_shifted.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_shifted, .-unsigned_shifted
	.globl	unsigned_step
	.type	unsigned_step, @function
unsigned_step:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x1, [x29, #16]
	ldr	w0, [x1]
	add	w0, w0, #1
	str	w0, [x1]
	b	.Lunsigned_step.1
.Lunsigned_step.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_step, .-unsigned_step
	.globl	unsigned_global
	.type	unsigned_global, @function
unsigned_global:
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
	adrp	x16, counter
	ldr	w1, [x16, :lo12:counter]
	sub	w0, w1, w0
	adrp	x16, counter
	str	w0, [x16, :lo12:counter]
	b	.Lunsigned_global.1
.Lunsigned_global.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_global, .-unsigned_global
	.globl	unsigned_slot
	.type	unsigned_slot, @function
unsigned_slot:
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
	ldr	w1, [x29, #24]
	mul	w0, w1, w0
	str	w0, [x29, #24]
	b	.Lunsigned_slot.1
.Lunsigned_slot.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_slot, .-unsigned_slot
	.globl	unsigned_postfix
	.type	unsigned_postfix, @function
unsigned_postfix:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x1, [x29, #16]
	ldr	w0, [x1]
	add	w2, w0, #1
	str	w2, [x1]
	b	.Lunsigned_postfix.1
.Lunsigned_postfix.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_postfix, .-unsigned_postfix
	.globl	unsigned_wide
	.type	unsigned_wide, @function
unsigned_wide:
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
	ldr	w2, [x1]
	add	x0, x2, x0
	str	w0, [x1]
	mov	w0, w0
	b	.Lunsigned_wide.1
.Lunsigned_wide.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_wide, .-unsigned_wide
	.globl	unsigned_narrow
	.type	unsigned_narrow, @function
unsigned_narrow:
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
	ldr	w0, [x29, #24]
	ldrh	w2, [x1]
	add	w0, w2, w0
	strh	w0, [x1]
	uxth	w0, w0
	b	.Lunsigned_narrow.1
.Lunsigned_narrow.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_narrow, .-unsigned_narrow
	.globl	signed_value
	.type	signed_value, @function
signed_value:
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
	ldr	w0, [x29, #24]
	ldr	w2, [x1]
	add	w0, w2, w0
	str	w0, [x1]
	sxtw	x0, w0
	b	.Lsigned_value.1
.Lsigned_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	signed_value, .-signed_value
	.globl	unsigned_plain
	.type	unsigned_plain, @function
unsigned_plain:
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
	b	.Lunsigned_plain.1
.Lunsigned_plain.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_plain, .-unsigned_plain
	.globl	called_value
	.type	called_value, @function
called_value:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	mix
	ldr	x1, [x29, #24]
	add	x0, x1, x0
	str	x0, [x29, #24]
	b	.Lcalled_value.1
.Lcalled_value.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	called_value, .-called_value
	.globl	product_element
	.type	product_element, @function
product_element:
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
	str	w2, [x29, #28]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	ldr	w2, [x29, #28]
	ldr	w3, [x0, w1, sxtw #2]
	mul	w3, w3, w2
	str	w3, [x0, w1, sxtw #2]
.Lproduct_element.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	product_element, .-product_element
	.globl	product_pointer
	.type	product_pointer, @function
product_pointer:
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
	ldr	x2, [x0]
	mul	x2, x2, x1
	str	x2, [x0]
	ldr	x0, [x29, #24]
	b	.Lproduct_pointer.1
.Lproduct_pointer.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	product_pointer, .-product_pointer
	.globl	product_value
	.type	product_value, @function
product_value:
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
	ldr	x2, [x1]
	mul	x0, x2, x0
	str	x0, [x1]
	b	.Lproduct_value.1
.Lproduct_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	product_value, .-product_value
	.globl	called_minus
	.type	called_minus, @function
called_minus:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	bl	mix
	ldr	x1, [x29, #24]
	sub	x0, x1, x0
	str	x0, [x29, #24]
	b	.Lcalled_minus.1
.Lcalled_minus.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	called_minus, .-called_minus
	.globl	product_char
	.type	product_char, @function
product_char:
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
	ldrb	w2, [x0]
	mul	w2, w2, w1
	strb	w2, [x0]
.Lproduct_char.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	product_char, .-product_char
	.bss
	.globl	counter
	.type	counter, @object
	.size	counter, 4
	.p2align 2
counter:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
