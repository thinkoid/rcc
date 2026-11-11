	.file	"far-slot.c"
	.text
	.globl	far
	.type	far, @function
far:
	.cfi_startproc
	sub	sp, sp, #36864
	.cfi_def_cfa_offset 36864
	sub	sp, sp, #32
	.cfi_def_cfa_offset 36896
	stp	x29, x30, [sp]
	.cfi_offset w29, -36896
	.cfi_offset w30, -36888
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	add	x16, x29, #36864
	str	w0, [x16, #20]
	add	x16, x29, #36864
	ldr	w0, [x16, #20]
	add	w0, w0, #1
	b	.Lfar.1
.Lfar.1:
	.cfi_def_cfa wsp, 36896
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #36864
	.cfi_def_cfa_offset 32
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	far, .-far
	.globl	near
	.type	near, @function
near:
	.cfi_startproc
	sub	sp, sp, #36864
	.cfi_def_cfa_offset 36864
	sub	sp, sp, #32
	.cfi_def_cfa_offset 36896
	stp	x29, x30, [sp]
	.cfi_offset w29, -36896
	.cfi_offset w30, -36888
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	b	.Lnear.1
.Lnear.1:
	.cfi_def_cfa wsp, 36896
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #36864
	.cfi_def_cfa_offset 32
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	near, .-near
	.section	.note.GNU-stack,"",@progbits
