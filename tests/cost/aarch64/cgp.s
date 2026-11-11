	.file	"cgp.c"
	.text
	.globl	empty_if
	.type	empty_if, @function
empty_if:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	wzr, [x29, #20]
	ldr	w0, [x29, #16]
	cbz	w0, .Lempty_if.2
.Lempty_if.2:
	ldr	w0, [x29, #16]
	cmp	w0, #1
	b.le	.Lempty_if.3
	mov	w0, #1
	str	w0, [x29, #20]
.Lempty_if.3:
	ldr	w0, [x29, #20]
	b	.Lempty_if.1
.Lempty_if.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	empty_if, .-empty_if
	.section	.note.GNU-stack,"",@progbits
