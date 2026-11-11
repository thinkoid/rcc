	.file	"call.c"
	.text
	.globl	call
	.type	call, @function
call:
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
	ldr	w0, [x29, #20]
	str	w0, [x29, #24]
	ldr	w0, [x29, #16]
	str	w0, [x29, #28]
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #28]
	bl	f
	b	.Lcall.1
.Lcall.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	call, .-call
	.section	.note.GNU-stack,"",@progbits
