	.file	"register-undef.c"
	.text
	.globl	which
	.type	which, @function
which:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	cmp	w19, #10
	b.ge	.Lwhich.2
	mov	w0, #1
	b	.Lwhich.1
.Lwhich.2:
	cmp	w19, #10
	b.lt	.Lwhich.3
	mov	w0, #2
	b	.Lwhich.1
.Lwhich.3:
	mov	w0, #3
	b	.Lwhich.1
.Lwhich.1:
	.cfi_def_cfa wsp, 32
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	which, .-which
	.section	.note.GNU-stack,"",@progbits
