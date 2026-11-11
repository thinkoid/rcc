	.file	"cast.c"
	.text
	.globl	signed_
	.type	signed_, @function
signed_:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	strb	w0, [x29, #20]
	ldrb	w0, [x29, #20]
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	sxtw	x0, w0
	str	x0, [x29, #32]
	ldr	x0, [x29, #32]
	b	.Lsigned_.1
.Lsigned_.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	signed_, .-signed_
	.globl	unsigned_
	.type	unsigned_, @function
unsigned_:
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
	strb	w0, [x29, #20]
	ldrb	w0, [x29, #20]
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	b	.Lunsigned_.1
.Lunsigned_.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	unsigned_, .-unsigned_
	.globl	same
	.type	same, @function
same:
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
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	b	.Lsame.1
.Lsame.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	same, .-same
	.section	.note.GNU-stack,"",@progbits
