	.file	"volatile.c"
	.text
	.globl	peek
	.type	peek, @function
peek:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	mov	w0, #1
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	w0, [x0]
	ldr	x0, [x29, #16]
	ldr	w0, [x0]
	ldr	x1, [x29, #16]
	ldr	w1, [x1]
	add	w0, w0, w1
	b	.Lpeek.1
.Lpeek.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	peek, .-peek
	.globl	poke
	.type	poke, @function
poke:
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
	mov	w1, #65
	strb	w1, [x0]
.Lpoke.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	poke, .-poke
	.globl	spin
	.type	spin, @function
spin:
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
	str	w0, [x29, #24]
	mov	w19, #0
.Lspin.2:
	ldr	w0, [x29, #24]
	cmp	w19, w0
	b.ge	.Lspin.3
	add	w19, w19, #1
	b	.Lspin.2
.Lspin.3:
	mov	w0, w19
	b	.Lspin.1
.Lspin.1:
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
	.size	spin, .-spin
	.section	.note.GNU-stack,"",@progbits
