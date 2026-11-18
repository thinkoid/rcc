	.file	"switch-direct.c"
	.text
	.globl	day
	.type	day, @function
day:
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
	sub	w0, w0, #1
	cmp	w0, #4
	b.hi	.Lday.6
	adrp	x16, .Lday.8
	add	x16, x16, :lo12:.Lday.8
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Lday.8:
	.quad	.Lday.2
	.quad	.Lday.3
	.quad	.Lday.4
	.quad	.Lday.6
	.quad	.Lday.5
	.text
.Lday.2:
	mov	w0, #10
	b	.Lday.1
.Lday.3:
	mov	w0, #20
	b	.Lday.1
.Lday.4:
	mov	w0, #30
	b	.Lday.1
.Lday.5:
	mov	w0, #50
	b	.Lday.1
.Lday.6:
	mov	w0, #0
	b	.Lday.1
.Lday.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	day, .-day
	.globl	digit
	.type	digit, @function
digit:
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
	cmp	w0, #3
	b.hi	.Ldigit.6
	adrp	x16, .Ldigit.7
	add	x16, x16, :lo12:.Ldigit.7
	ldr	x16, [x16, w0, uxtw #3]
	br	x16
	.data
	.p2align 3
.Ldigit.7:
	.quad	.Ldigit.2
	.quad	.Ldigit.3
	.quad	.Ldigit.4
	.quad	.Ldigit.5
	.text
.Ldigit.2:
	mov	w0, #7
	b	.Ldigit.1
.Ldigit.3:
	mov	w0, #6
	b	.Ldigit.1
.Ldigit.4:
	mov	w0, #5
	b	.Ldigit.1
.Ldigit.5:
	mov	w0, #4
	b	.Ldigit.1
.Ldigit.6:
	mov	w0, #-1
	b	.Ldigit.1
.Ldigit.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	digit, .-digit
	.section	.note.GNU-stack,"",@progbits
