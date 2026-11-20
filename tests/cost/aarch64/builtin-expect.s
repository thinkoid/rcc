	.file	"builtin-expect.c"
	.text
	.globl	pick
	.type	pick, @function
pick:
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
	cmp	w0, #0
	cset	w0, lt
	cmp	w0, #0
	cset	w0, eq
	cmp	w0, #0
	cset	w0, eq
	cbz	x0, .Lpick.2
	mov	w0, #0
	b	.Lpick.1
.Lpick.2:
	ldr	w0, [x29, #16]
	b	.Lpick.1
.Lpick.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	wide
	.type	wide, @function
wide:
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
	cmp	w0, #0
	cset	w0, eq
	cmp	w0, #0
	cset	w0, eq
	b	.Lwide.1
.Lwide.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	wide, .-wide
	.section	.note.GNU-stack,"",@progbits
