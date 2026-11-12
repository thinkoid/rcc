	.file	"register-walk.c"
	.text
	.globl	copy
	.type	copy, @function
copy:
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
	mov	x19, x0
	mov	x20, x1
	mov	x21, x19
.Lcopy.2:
	ldrb	w0, [x20], #1
	strb	w0, [x19], #1
	cbz	w0, .Lcopy.3
	b	.Lcopy.2
.Lcopy.3:
	mov	x0, x21
	b	.Lcopy.1
.Lcopy.1:
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
	.size	copy, .-copy
	.globl	length
	.type	length, @function
length:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -16
	.cfi_offset w20, -8
	mov	x19, x0
	mov	w20, #0
.Llength.2:
	ldrb	w0, [x19], #1
	cbz	w0, .Llength.3
	add	w20, w20, #1
	b	.Llength.2
.Llength.3:
	mov	w0, w20
	b	.Llength.1
.Llength.1:
	.cfi_def_cfa wsp, 32
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	length, .-length
	.globl	compare
	.type	compare, @function
compare:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -16
	.cfi_offset w20, -8
	mov	x19, x0
	mov	x20, x1
.Lcompare.2:
	ldrb	w0, [x19]
	ldrb	w1, [x20], #1
	cmp	w0, w1
	b.ne	.Lcompare.3
	ldrb	w0, [x19], #1
	cmp	w0, #0
	b.ne	.Lcompare.4
	mov	w0, #0
	b	.Lcompare.1
.Lcompare.4:
	b	.Lcompare.2
.Lcompare.3:
	ldrb	w0, [x19]
	ldrb	w1, [x20, #-1]!
	sub	w0, w0, w1
	b	.Lcompare.1
.Lcompare.1:
	.cfi_def_cfa wsp, 32
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	compare, .-compare
	.section	.note.GNU-stack,"",@progbits
