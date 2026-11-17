	.file	"div-busy.c"
	.text
	.globl	div_busy
	.type	div_busy, @function
div_busy:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	b	.Ldiv_busy.1
.Ldiv_busy.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div_busy, .-div_busy
	.globl	rem_busy
	.type	rem_busy, @function
rem_busy:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w3, w1, w2
	msub	w1, w3, w2, w1
	add	w0, w0, w1
	b	.Lrem_busy.1
.Lrem_busy.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	rem_busy, .-rem_busy
	.globl	div_deeper
	.type	div_deeper, @function
div_deeper:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	sdiv	w1, w1, w2
	add	w1, w1, #1
	add	w0, w0, w1
	b	.Ldiv_deeper.1
.Ldiv_deeper.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div_deeper, .-div_deeper
	.globl	div_held
	.type	div_held, @function
div_held:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	ldr	w17, [x29, #64]
	str	w17, [x29, #48]
	ldr	w17, [x29, #72]
	str	w17, [x29, #52]
	ldr	w17, [x29, #80]
	str	w17, [x29, #56]
	ldr	w17, [x29, #88]
	str	w17, [x29, #60]
	str	w0, [x29, #16]
	str	w1, [x29, #20]
	str	w2, [x29, #24]
	str	w3, [x29, #28]
	str	w4, [x29, #32]
	str	w5, [x29, #36]
	str	w6, [x29, #40]
	str	w7, [x29, #44]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	ldr	w3, [x29, #28]
	ldr	w4, [x29, #32]
	ldr	w5, [x29, #36]
	ldr	w6, [x29, #40]
	ldr	w7, [x29, #44]
	ldr	w9, [x29, #48]
	ldr	w10, [x29, #52]
	sdiv	w9, w9, w10
	add	w7, w7, w9
	add	w6, w6, w7
	add	w5, w5, w6
	add	w4, w4, w5
	add	w3, w3, w4
	add	w2, w2, w3
	add	w1, w1, w2
	add	w0, w0, w1
	ldr	w1, [x29, #56]
	ldr	w2, [x29, #60]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	b	.Ldiv_held.1
.Ldiv_held.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	div_held, .-div_held
	.section	.note.GNU-stack,"",@progbits
