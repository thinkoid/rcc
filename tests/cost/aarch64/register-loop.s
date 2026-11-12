	.file	"register-loop.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	x21, [x29, #32]
	.cfi_offset w21, -16
	mov	w19, w0
	mov	w20, #0
	mov	w21, #0
.Lsum.2:
	cmp	w21, w19
	b.ge	.Lsum.4
	add	w0, w20, w21
	mov	w20, w0
.Lsum.3:
	add	w0, w21, #1
	mov	w21, w0
	b	.Lsum.2
.Lsum.4:
	mov	w0, w20
	b	.Lsum.1
.Lsum.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldr	x21, [x29, #32]
	.cfi_restore w21
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sum, .-sum
	.section	.note.GNU-stack,"",@progbits
