	.file	"struct-static-completed.c"
	.text
	.globl	y_of
	.type	y_of, @function
y_of:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, origin+4
	ldr	w0, [x16, :lo12:origin+4]
	b	.Ly_of.1
.Ly_of.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	y_of, .-y_of
	.bss
	.type	origin, @object
	.size	origin, 8
	.p2align 2
origin:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
