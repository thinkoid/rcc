	.file	"zero-test.c"
	.text
	.globl	never
	.type	never, @function
never:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #0
	cbz	w0, .Lnever.2
	mov	w0, #1
	b	.Lnever.1
.Lnever.2:
	mov	w0, #2
	b	.Lnever.1
.Lnever.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	never, .-never
	.globl	not_zero
	.type	not_zero, @function
not_zero:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	b	.Lnot_zero.1
.Lnot_zero.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	not_zero, .-not_zero
	.section	.note.GNU-stack,"",@progbits
