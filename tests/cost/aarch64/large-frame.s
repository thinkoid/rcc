	.file	"large-frame.c"
	.text
	.globl	large
	.type	large, @function
large:
	.cfi_startproc
	sub	sp, sp, #8192
	.cfi_def_cfa_offset 8192
	sub	sp, sp, #16
	.cfi_def_cfa_offset 8208
	stp	x29, x30, [sp]
	.cfi_offset w29, -8208
	.cfi_offset w30, -8200
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Llarge.1
.Llarge.1:
	.cfi_def_cfa wsp, 8208
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #8192
	.cfi_def_cfa_offset 16
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	large, .-large
	.globl	aligned
	.type	aligned, @function
aligned:
	.cfi_startproc
	sub	sp, sp, #8192
	.cfi_def_cfa_offset 8192
	stp	x29, x30, [sp]
	.cfi_offset w29, -8192
	.cfi_offset w30, -8184
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Laligned.1
.Laligned.1:
	.cfi_def_cfa wsp, 8192
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #8192
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	aligned, .-aligned
	.section	.note.GNU-stack,"",@progbits
