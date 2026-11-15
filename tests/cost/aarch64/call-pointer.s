	.file	"call-pointer.c"
	.text
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
	str	w2, [x29, #28]
	ldr	x0, [x29, #16]
	str	x0, [x29, #32]
	ldr	w0, [x29, #28]
	str	w0, [x29, #40]
	ldr	w0, [x29, #24]
	str	w0, [x29, #44]
	ldr	w0, [x29, #40]
	ldr	w1, [x29, #44]
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
	.section	.note.GNU-stack,"",@progbits
