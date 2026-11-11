	.file	"constant-in-loop.c"
	.text
	.globl	constant_in_loop
	.type	constant_in_loop, @function
constant_in_loop:
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
	str	wzr, [x29, #28]
.Lconstant_in_loop.2:
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #24]
	cmp	w0, w1
	b.ge	.Lconstant_in_loop.4
	ldr	x0, [x29, #16]
	mov	x1, #52719
	movk	x1, #0x89ab, lsl #16
	movk	x1, #0x4567, lsl #32
	movk	x1, #0x123, lsl #48
	add	x0, x0, x1
	str	x0, [x29, #16]
.Lconstant_in_loop.3:
	ldr	w0, [x29, #28]
	add	w0, w0, #1
	str	w0, [x29, #28]
	b	.Lconstant_in_loop.2
.Lconstant_in_loop.4:
	ldr	x0, [x29, #16]
	b	.Lconstant_in_loop.1
.Lconstant_in_loop.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	constant_in_loop, .-constant_in_loop
	.section	.note.GNU-stack,"",@progbits
