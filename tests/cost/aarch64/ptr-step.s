	.file	"ptr-step.c"
	.text
	.globl	step
	.type	step, @function
step:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	add	x0, x0, #4
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	sub	x0, x0, #8
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	b	.Lstep.1
.Lstep.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	step, .-step
	.globl	in_value
	.type	in_value, @function
in_value:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	sub	x0, x0, #8
	str	x0, [x29, #16]
	b	.Lin_value.1
.Lin_value.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_value, .-in_value
	.section	.note.GNU-stack,"",@progbits
