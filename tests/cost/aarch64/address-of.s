	.file	"address-of.c"
	.text
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
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	add	x0, x29, #20
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	ldr	w0, [x0]
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
	.section	.note.GNU-stack,"",@progbits
