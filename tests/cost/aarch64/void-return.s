	.file	"void-return.c"
	.text
	.globl	tail
	.type	tail, @function
tail:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	g
	b	.Ltail.1
.Ltail.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	tail, .-tail
	.globl	pick
	.type	pick, @function
pick:
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
	cbz	w0, .Lpick.2
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	bl	h
	b	.Lpick.1
.Lpick.2:
	bl	g
.Lpick.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pick, .-pick
	.section	.note.GNU-stack,"",@progbits
