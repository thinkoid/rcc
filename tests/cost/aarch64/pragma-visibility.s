	.file	"pragma-visibility.c"
	.text
	.globl	shown_value
	.type	shown_value, @function
shown_value:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, shown
	ldr	w0, [x16, :lo12:shown]
	b	.Lshown_value.1
.Lshown_value.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shown_value, .-shown_value
	.type	twice, @function
twice:
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
	ldr	w1, [x29, #16]
	add	w0, w0, w1
	b	.Ltwice.1
.Ltwice.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	twice, .-twice
	.globl	twice_shown
	.type	twice_shown, @function
twice_shown:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, shown
	ldr	w0, [x16, :lo12:shown]
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	bl	twice
	b	.Ltwice_shown.1
.Ltwice_shown.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	twice_shown, .-twice_shown
	.data
	.globl	shown
	.type	shown, @object
	.size	shown, 4
	.p2align 2
shown:
	.long	1
	.section	.note.GNU-stack,"",@progbits
