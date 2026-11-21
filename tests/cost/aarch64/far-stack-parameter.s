	.file	"far-stack-parameter.c"
	.text
	.globl	eleventh
	.type	eleventh, @function
eleventh:
	.cfi_startproc
	sub	sp, sp, #16773120
	.cfi_def_cfa_offset 16773120
	sub	sp, sp, #4080
	.cfi_def_cfa_offset 16777200
	stp	x29, x30, [sp]
	.cfi_offset w29, -16777200
	.cfi_offset w30, -16777192
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	str	x3, [x29, #40]
	str	x4, [x29, #48]
	str	x5, [x29, #56]
	str	x6, [x29, #64]
	str	x7, [x29, #72]
	add	x16, x29, #16773120
	ldr	x17, [x16, #4080]
	str	x17, [x29, #80]
	add	x16, x29, #16773120
	ldr	x17, [x16, #4088]
	str	x17, [x29, #88]
	mov	x16, #16777216
	add	x16, x29, x16
	ldr	x17, [x16]
	str	x17, [x29, #96]
	ldr	x0, [x29, #96]
	b	.Leleventh.1
.Leleventh.1:
	.cfi_def_cfa wsp, 16777200
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16773120
	.cfi_def_cfa_offset 4080
	add	sp, sp, #4080
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	eleventh, .-eleventh
	.globl	eleventh_register
	.type	eleventh_register, @function
eleventh_register:
	.cfi_startproc
	sub	sp, sp, #16773120
	.cfi_def_cfa_offset 16773120
	sub	sp, sp, #4080
	.cfi_def_cfa_offset 16777200
	stp	x29, x30, [sp]
	.cfi_offset w29, -16777200
	.cfi_offset w30, -16777192
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16777184
	str	x0, [x29, #24]
	str	x1, [x29, #32]
	str	x2, [x29, #40]
	str	x3, [x29, #48]
	str	x4, [x29, #56]
	str	x5, [x29, #64]
	str	x6, [x29, #72]
	str	x7, [x29, #80]
	add	x16, x29, #16773120
	ldr	x17, [x16, #4080]
	str	x17, [x29, #88]
	add	x16, x29, #16773120
	ldr	x17, [x16, #4088]
	str	x17, [x29, #96]
	mov	x16, #16777216
	add	x16, x29, x16
	ldr	x19, [x16]
	mov	x0, x19
	b	.Leleventh_register.1
.Leleventh_register.1:
	.cfi_def_cfa wsp, 16777200
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16773120
	.cfi_def_cfa_offset 4080
	add	sp, sp, #4080
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	eleventh_register, .-eleventh_register
	.section	.note.GNU-stack,"",@progbits
