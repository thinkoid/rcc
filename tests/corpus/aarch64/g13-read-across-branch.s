	.file	"g13-read-across-branch.c"
	.text
	.globl	pick
	.type	pick, @function
pick:
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
	mov	w19, w0
	str	w1, [x29, #24]
	str	x19, [x29, #32]
	ldr	w0, [x29, #24]
	cbz	w0, .Lpick.2
	mov	w0, w19
	add	w19, w19, #1
	b	.Lpick.3
.Lpick.2:
	mov	w0, #2
.Lpick.3:
	ldr	x1, [x29, #32]
	add	w1, w1, w0
	mov	w0, w1
	b	.Lpick.1
.Lpick.1:
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
	.size	pick, .-pick
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #5
	str	w0, [x29, #16]
	mov	w0, #1
	str	w0, [x29, #20]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	bl	pick
	mov	w1, #16
	mul	w0, w0, w1
	str	x0, [x29, #24]
	mov	w0, #5
	str	w0, [x29, #32]
	str	wzr, [x29, #36]
	ldr	w0, [x29, #32]
	ldr	w1, [x29, #36]
	bl	pick
	ldr	x1, [x29, #24]
	add	w1, w1, w0
	mov	w0, w1
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
