	.file	"variadic-volatile-list.c"
	.text
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	sub	sp, sp, #288
	.cfi_def_cfa_offset 288
	stp	x29, x30, [sp]
	.cfi_offset w29, -288
	.cfi_offset w30, -280
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x0, x1, [x29, #16]
	stp	x2, x3, [x29, #32]
	stp	x4, x5, [x29, #48]
	stp	x6, x7, [x29, #64]
	stp	q0, q1, [x29, #80]
	stp	q2, q3, [x29, #112]
	stp	q4, q5, [x29, #144]
	stp	q6, q7, [x29, #176]
	add	x16, x29, #288
	str	x16, [x29, #208]
	str	w0, [x29, #216]
	add	x0, x29, #232
	str	x0, [x29, #264]
	ldr	x0, [x29, #208]
	str	x0, [x29, #232]
	add	x0, x29, #80
	str	x0, [x29, #240]
	add	x0, x29, #208
	str	x0, [x29, #248]
	mov	w0, #-56
	str	w0, [x29, #256]
	mov	w0, #-128
	str	w0, [x29, #260]
	ldr	x0, [x29, #264]
	str	x0, [x29, #224]
	ldr	x0, [x29, #224]
	ldr	w0, [x0, #24]
	cmp	w0, #0
	b.ge	.Lfirst.2
	ldr	x0, [x29, #224]
	ldr	w1, [x0, #24]
	add	w1, w1, #8
	str	w1, [x0, #24]
	ldr	x0, [x29, #224]
	ldr	x0, [x0, #8]
	ldr	x1, [x29, #224]
	ldr	w1, [x1, #24]
	sub	w1, w1, #8
	add	x0, x0, w1, sxtw
	b	.Lfirst.3
.Lfirst.2:
	ldr	x0, [x29, #224]
	ldr	x1, [x29, #224]
	ldr	x1, [x1]
	add	x1, x1, #8
	str	x1, [x0]
	ldr	x0, [x29, #224]
	ldr	x0, [x0]
	sub	x0, x0, #8
.Lfirst.3:
	ldr	w0, [x0]
	str	w0, [x29, #272]
	add	x0, x29, #232
	ldr	w0, [x29, #272]
	b	.Lfirst.1
.Lfirst.1:
	.cfi_def_cfa wsp, 288
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #288
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	first, .-first
	.section	.note.GNU-stack,"",@progbits
