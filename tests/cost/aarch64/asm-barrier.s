	.file	"asm-barrier.c"
	.text
	.globl	stored
	.type	stored, @function
stored:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	stored_null
	.type	stored_null, @function
stored_null:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored_null.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_null, .-stored_null
	.globl	stored_cc
	.type	stored_cc, @function
stored_cc:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored_cc.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_cc, .-stored_cc
	.globl	stored_cc_null
	.type	stored_cc_null, @function
stored_cc_null:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored_cc_null.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_cc_null, .-stored_cc_null
	.globl	stored_bare
	.type	stored_bare, @function
stored_bare:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored_bare.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_bare, .-stored_bare
	.globl	stored_bare_null
	.type	stored_bare_null, @function
stored_bare_null:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored_bare_null.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored_bare_null, .-stored_bare_null
	.globl	fenced
	.type	fenced, @function
fenced:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lfenced.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fenced, .-fenced
	.globl	fenced_null
	.type	fenced_null, @function
fenced_null:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lfenced_null.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fenced_null, .-fenced_null
	.globl	held
	.type	held, @function
held:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, pending
	ldr	w0, [x16, :lo12:pending]
	add	w0, w0, #1
	adrp	x16, pending
	str	w0, [x16, :lo12:pending]
	mov	w0, #0
.Lheld.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	held_null
	.type	held_null, @function
held_null:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, pending
	ldr	w0, [x16, :lo12:pending]
	add	w0, w0, #1
	adrp	x16, pending
	str	w0, [x16, :lo12:pending]
	mov	w0, #0
.Lheld_null.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held_null, .-held_null
	.globl	valued
	.type	valued, @function
valued:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, g
	ldr	w0, [x16, :lo12:g]
	add	w0, w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
	mov	w0, #0
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Lvalued.1
.Lvalued.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	valued, .-valued
	.globl	valued_null
	.type	valued_null, @function
valued_null:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, g
	ldr	w0, [x16, :lo12:g]
	add	w0, w0, #1
	adrp	x16, g
	str	w0, [x16, :lo12:g]
	mov	w0, #0
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Lvalued_null.1
.Lvalued_null.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	valued_null, .-valued_null
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	pending
	.type	pending, @object
	.size	pending, 4
	.p2align 2
pending:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
