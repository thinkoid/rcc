	.file	"utf8-string.c"
	.text
	.globl	first
	.type	first, @function
first:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, s
	ldrb	w0, [x16, :lo12:s]
	b	.Lfirst.1
.Lfirst.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	first, .-first
	.data
	.globl	s
	.type	s, @object
	.size	s, 3
s:
	.string	"\303\251"
	.section	.note.GNU-stack,"",@progbits
