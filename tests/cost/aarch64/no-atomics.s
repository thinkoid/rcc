	.file	"no-atomics.c"
	.text
	.globl	no_atomics
	.type	no_atomics, @function
no_atomics:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	b	.Lno_atomics.1
.Lno_atomics.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	no_atomics, .-no_atomics
	.section	.note.GNU-stack,"",@progbits
