	.file	"statement-expression.c"
	.text
	.globl	interrupts_off
	.type	interrupts_off, @function
interrupts_off:
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
.Linterrupts_off.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	interrupts_off, .-interrupts_off
	.globl	assigned
	.type	assigned, @function
assigned:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	f
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	str	w0, [x29, #16]
	ldr	w0, [x29, #16]
	b	.Lassigned.1
.Lassigned.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	assigned, .-assigned
	.globl	held
	.type	held, @function
held:
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
	str	x0, [x29, #24]
	bl	g
	ldr	w0, [x29, #20]
	ldr	x1, [x29, #24]
	add	w0, w1, w0
	b	.Lheld.1
.Lheld.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	held, .-held
	.globl	argument
	.type	argument, @function
argument:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	str	w0, [x29, #16]
	bl	g
	mov	w0, #2
	str	w0, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #24]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #24]
	bl	three
	b	.Largument.1
.Largument.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	argument, .-argument
	.globl	declared
	.type	declared, @function
declared:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #1
	str	w0, [x29, #16]
	bl	f
	str	w0, [x29, #24]
	ldr	w0, [x29, #24]
	mov	w1, #2
	mul	w0, w0, w1
	str	w0, [x29, #20]
	mov	w0, #3
	str	w0, [x29, #28]
	ldr	w0, [x29, #16]
	ldr	w1, [x29, #20]
	ldr	w2, [x29, #28]
	bl	three
	b	.Ldeclared.1
.Ldeclared.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	declared, .-declared
	.globl	tested
	.type	tested, @function
tested:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	bl	g
	ldr	w0, [x29, #16]
	cbz	w0, .Ltested.2
	mov	w0, #1
	b	.Ltested.1
.Ltested.2:
	mov	w0, #0
	b	.Ltested.1
.Ltested.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	tested, .-tested
	.globl	copied
	.type	copied, @function
copied:
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
	bl	f
	mov	w19, w0
	mov	w0, w19
	b	.Lcopied.1
.Lcopied.1:
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
	.size	copied, .-copied
	.globl	bare
	.type	bare, @function
bare:
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
	add	w0, w0, w1
	b	.Lbare.1
.Lbare.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	bare, .-bare
	.globl	local
	.type	local, @function
local:
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
	str	w1, [x29, #24]
	ldr	w1, [x29, #24]
	add	w0, w0, w1
	b	.Llocal.1
.Llocal.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	local, .-local
	.bss
	.globl	pending
	.type	pending, @object
	.size	pending, 4
	.p2align 2
pending:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
