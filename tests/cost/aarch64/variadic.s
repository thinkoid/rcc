	.file	"variadic.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	sub	sp, sp, #272
	.cfi_def_cfa_offset 272
	stp	x29, x30, [sp]
	.cfi_offset w29, -272
	.cfi_offset w30, -264
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
	add	x16, x29, #272
	str	x16, [x29, #208]
	str	w0, [x29, #216]
	str	xzr, [x29, #256]
	ldr	x0, [x29, #208]
	str	x0, [x29, #224]
	add	x0, x29, #80
	str	x0, [x29, #232]
	add	x0, x29, #208
	str	x0, [x29, #240]
	mov	w0, #-56
	str	w0, [x29, #248]
	mov	w0, #-128
	str	w0, [x29, #252]
.Lsum.2:
	ldr	w0, [x29, #216]
	sub	w1, w0, #1
	str	w1, [x29, #216]
	cmp	w0, #0
	b.le	.Lsum.3
	ldr	w0, [x29, #248]
	cmp	w0, #0
	b.ge	.Lsum.4
	ldr	w0, [x29, #248]
	add	w0, w0, #8
	str	w0, [x29, #248]
	ldr	x0, [x29, #232]
	ldr	w1, [x29, #248]
	sub	w1, w1, #8
	add	x0, x0, w1, sxtw
	b	.Lsum.5
.Lsum.4:
	ldr	x0, [x29, #224]
	add	x0, x0, #8
	str	x0, [x29, #224]
	ldr	x0, [x29, #224]
	sub	x0, x0, #8
.Lsum.5:
	ldr	x0, [x0]
	ldr	x1, [x29, #256]
	add	x1, x1, x0
	str	x1, [x29, #256]
	b	.Lsum.2
.Lsum.3:
	add	x0, x29, #224
	ldr	x0, [x29, #256]
	b	.Lsum.1
.Lsum.1:
	.cfi_def_cfa wsp, 272
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #272
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sum, .-sum
	.globl	twice
	.type	twice, @function
twice:
	.cfi_startproc
	sub	sp, sp, #304
	.cfi_def_cfa_offset 304
	stp	x29, x30, [sp]
	.cfi_offset w29, -304
	.cfi_offset w30, -296
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
	add	x16, x29, #304
	str	x16, [x29, #208]
	str	w0, [x29, #216]
	str	wzr, [x29, #288]
	ldr	x0, [x29, #208]
	str	x0, [x29, #224]
	add	x0, x29, #80
	str	x0, [x29, #232]
	add	x0, x29, #208
	str	x0, [x29, #240]
	mov	w0, #-56
	str	w0, [x29, #248]
	mov	w0, #-128
	str	w0, [x29, #252]
	ldr	x17, [x29, #224]
	str	x17, [x29, #256]
	ldr	x17, [x29, #232]
	str	x17, [x29, #264]
	ldr	x17, [x29, #240]
	str	x17, [x29, #272]
	ldr	x17, [x29, #248]
	str	x17, [x29, #280]
	ldr	w0, [x29, #216]
	str	w0, [x29, #292]
.Ltwice.2:
	ldr	w0, [x29, #292]
	cmp	w0, #0
	b.le	.Ltwice.4
	ldr	w0, [x29, #248]
	cmp	w0, #0
	b.ge	.Ltwice.5
	ldr	w0, [x29, #248]
	add	w0, w0, #8
	str	w0, [x29, #248]
	ldr	x0, [x29, #232]
	ldr	w1, [x29, #248]
	sub	w1, w1, #8
	add	x0, x0, w1, sxtw
	b	.Ltwice.6
.Ltwice.5:
	ldr	x0, [x29, #224]
	add	x0, x0, #8
	str	x0, [x29, #224]
	ldr	x0, [x29, #224]
	sub	x0, x0, #8
.Ltwice.6:
	ldr	w0, [x0]
	str	x0, [x29, #296]
	ldr	w0, [x29, #280]
	cmp	w0, #0
	b.ge	.Ltwice.7
	ldr	w0, [x29, #280]
	add	w0, w0, #8
	str	w0, [x29, #280]
	ldr	x0, [x29, #264]
	ldr	w1, [x29, #280]
	sub	w1, w1, #8
	add	x0, x0, w1, sxtw
	b	.Ltwice.8
.Ltwice.7:
	ldr	x0, [x29, #256]
	add	x0, x0, #8
	str	x0, [x29, #256]
	ldr	x0, [x29, #256]
	sub	x0, x0, #8
.Ltwice.8:
	ldr	w0, [x0]
	ldr	x1, [x29, #296]
	add	w1, w1, w0
	ldr	w0, [x29, #288]
	add	w0, w0, w1
	str	w0, [x29, #288]
	ldr	w0, [x29, #292]
	sub	w0, w0, #1
	str	w0, [x29, #292]
	b	.Ltwice.2
.Ltwice.4:
	add	x0, x29, #256
	add	x0, x29, #224
	ldr	w0, [x29, #288]
	b	.Ltwice.1
.Ltwice.1:
	.cfi_def_cfa wsp, 304
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #304
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	twice, .-twice
	.globl	stepped
	.type	stepped, @function
stepped:
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
	str	xzr, [x29, #40]
.Lstepped.2:
	ldr	w0, [x29, #24]
	sub	w1, w0, #1
	str	w1, [x29, #24]
	cmp	w0, #0
	b.le	.Lstepped.3
	ldr	x0, [x29, #16]
	add	x1, x0, #32
	str	x1, [x29, #16]
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	ldr	w0, [x0, #24]
	cmp	w0, #0
	b.ge	.Lstepped.4
	ldr	x0, [x29, #32]
	ldr	w1, [x0, #24]
	add	w1, w1, #8
	str	w1, [x0, #24]
	ldr	x0, [x29, #32]
	ldr	x0, [x0, #8]
	ldr	x1, [x29, #32]
	ldr	w1, [x1, #24]
	sub	w1, w1, #8
	add	x0, x0, w1, sxtw
	b	.Lstepped.5
.Lstepped.4:
	ldr	x0, [x29, #32]
	ldr	x1, [x29, #32]
	ldr	x1, [x1]
	add	x1, x1, #8
	str	x1, [x0]
	ldr	x0, [x29, #32]
	ldr	x0, [x0]
	sub	x0, x0, #8
.Lstepped.5:
	ldr	x0, [x0]
	ldr	x1, [x29, #40]
	add	x1, x1, x0
	str	x1, [x29, #40]
	b	.Lstepped.2
.Lstepped.3:
	ldr	x0, [x29, #40]
	b	.Lstepped.1
.Lstepped.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stepped, .-stepped
	.globl	print
	.type	print, @function
print:
	.cfi_startproc
	sub	sp, sp, #304
	.cfi_def_cfa_offset 304
	stp	x29, x30, [sp]
	.cfi_offset w29, -304
	.cfi_offset w30, -296
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
	add	x16, x29, #304
	str	x16, [x29, #208]
	str	x0, [x29, #216]
	ldr	x0, [x29, #208]
	str	x0, [x29, #224]
	add	x0, x29, #80
	str	x0, [x29, #232]
	add	x0, x29, #208
	str	x0, [x29, #240]
	mov	w0, #-56
	str	w0, [x29, #248]
	mov	w0, #-128
	str	w0, [x29, #252]
	ldr	x0, [x29, #216]
	str	x0, [x29, #264]
	ldr	x17, [x29, #224]
	str	x17, [x29, #272]
	ldr	x17, [x29, #232]
	str	x17, [x29, #280]
	ldr	x17, [x29, #240]
	str	x17, [x29, #288]
	ldr	x17, [x29, #248]
	str	x17, [x29, #296]
	ldr	x0, [x29, #264]
	add	x1, x29, #272
	bl	vprintf
	str	w0, [x29, #256]
	add	x0, x29, #224
	ldr	w0, [x29, #256]
	b	.Lprint.1
.Lprint.1:
	.cfi_def_cfa wsp, 304
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #304
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	print, .-print
	.section	.note.GNU-stack,"",@progbits
