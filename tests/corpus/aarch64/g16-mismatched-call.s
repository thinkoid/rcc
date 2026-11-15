	.file	"g16-mismatched-call.c"
	.text
	.globl	wide
	.type	wide, @function
wide:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #300
	b	.Lwide.1
.Lwide.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	wide, .-wide
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, wide
	add	x0, x0, :lo12:wide
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	str	x0, [x29, #24]
	ldr	x16, [x29, #24]
	blr	x16
	uxtb	w0, w0
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
