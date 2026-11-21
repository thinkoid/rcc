	.file	"register-names.c"
	.text
	.globl	h1
	.type	h1, @function
h1:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	b	.Lh1.1
.Lh1.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	h1, .-h1
	.globl	x0
	.type	x0, @function
x0:
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
	adrp	x16, x1
	ldr	w1, [x16, :lo12:x1]
	add	w0, w0, w1
	b	.Lx0.1
.Lx0.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	x0, .-x0
	.globl	w2
	.type	w2, @function
w2:
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
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	mul	w0, w0, w1
	b	.Lw2.1
.Lw2.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	w2, .-w2
	.globl	sp
	.type	sp, @function
sp:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, x1
	ldr	w0, [x16, :lo12:x1]
	mov	w1, #2
	mul	w0, w0, w1
	b	.Lsp.1
.Lsp.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sp, .-sp
	.globl	Lr
	.type	Lr, @function
Lr:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #7
	b	.LLr.1
.LLr.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	Lr, .-Lr
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	"h1"
	cmp	w0, #1
	b.eq	.Lmain.2
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	mov	w0, #2
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	bl	"x0"
	cmp	w0, #7
	b.eq	.Lmain.3
	mov	w0, #102
	b	.Lmain.1
.Lmain.3:
	mov	w0, #3
	str	w0, [x29, #20]
	mov	w0, #4
	str	w0, [x29, #24]
	ldr	w0, [x29, #20]
	ldr	w1, [x29, #24]
	bl	"w2"
	cmp	w0, #12
	b.eq	.Lmain.4
	mov	w0, #103
	b	.Lmain.1
.Lmain.4:
	mov	w0, #10
	adrp	x16, x1
	str	w0, [x16, :lo12:x1]
	bl	"sp"
	cmp	w0, #20
	b.eq	.Lmain.5
	mov	w0, #104
	b	.Lmain.1
.Lmain.5:
	bl	"Lr"
	cmp	w0, #7
	b.eq	.Lmain.6
	mov	w0, #105
	b	.Lmain.1
.Lmain.6:
	mov	w0, #42
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
	.globl	x1
	.type	x1, @object
	.size	x1, 4
	.p2align 2
x1:
	.long	5
	.section	.note.GNU-stack,"",@progbits
