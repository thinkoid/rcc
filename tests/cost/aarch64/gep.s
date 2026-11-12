	.file	"gep.c"
	.text
	.globl	gep
	.type	gep, @function
gep:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	add	x0, x29, #20
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #2
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	b	.Lgep.1
.Lgep.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	gep, .-gep
	.section	.note.GNU-stack,"",@progbits
