	.file	"register-parameter.c"
	.text
	.globl	parameter
	.type	parameter, @function
parameter:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -16
	.cfi_offset w20, -8
	mov	w19, w0
	mov	w20, w1
	sub	w0, w19, w20
	add	w0, w0, w19
	b	.Lparameter.1
.Lparameter.1:
	.cfi_def_cfa wsp, 32
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	parameter, .-parameter
	.section	.note.GNU-stack,"",@progbits
