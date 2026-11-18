	.file	"register-copy.c"
	.text
	.globl	stored
	.type	stored, @function
stored:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	add	w0, w19, w0
	adrp	x16, g
	str	w0, [x16, :lo12:g]
.Lstored.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored, .-stored
	.globl	local
	.type	local, @function
local:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	f
	mul	w0, w19, w0
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	b	.Llocal.1
.Llocal.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	argument
	.type	argument, @function
argument:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	f
	and	w0, w19, w0
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
.Largument.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	argument, .-argument
	.globl	destination
	.type	destination, @function
destination:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	w0, [x29, #32]
	str	w1, [x29, #36]
	ldr	w19, [x29, #32]
	ldr	w0, [x29, #36]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	orr	w20, w19, w0
	eor	w0, w20, w19
	b	.Ldestination.1
.Ldestination.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	destination, .-destination
	.globl	chained
	.type	chained, @function
chained:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	eor	w0, w19, w0
	ldr	w1, [x29, #32]
	add	w0, w0, w1
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	b	.Lchained.1
.Lchained.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	chained, .-chained
	.globl	both
	.type	both, @function
both:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	stp	x19, x20, [x29, #16]
	.cfi_offset w19, -32
	.cfi_offset w20, -24
	str	w0, [x29, #32]
	str	w1, [x29, #36]
	ldr	w19, [x29, #32]
	ldr	w20, [x29, #36]
	add	w0, w19, w20
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	b	.Lboth.1
.Lboth.1:
	.cfi_def_cfa wsp, 48
	ldp	x19, x20, [x29, #16]
	.cfi_restore w19
	.cfi_restore w20
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	in_place
	.type	in_place, @function
in_place:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	bl	f
	add	w19, w19, w0
	mov	w0, w19
	b	.Lin_place.1
.Lin_place.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_place, .-in_place
	.globl	named
	.type	named, @function
named:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #32]
	mul	w0, w0, w1
	add	w0, w19, w0
	b	.Lnamed.1
.Lnamed.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	named, .-named
	.globl	reserved
	.type	reserved, @function
reserved:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	str	w2, [x29, #32]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	f
	add	w0, w19, w0
	ldr	w1, [x29, #28]
	ldr	w2, [x29, #32]
	sdiv	w1, w1, w2
	add	w0, w0, w1
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	b	.Lreserved.1
.Lreserved.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	reserved, .-reserved
	.globl	minus
	.type	minus, @function
minus:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	ldr	w0, [x29, #28]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	f
	sub	w0, w19, w0
	str	w0, [x29, #32]
	ldr	w0, [x29, #32]
	b	.Lminus.1
.Lminus.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	minus, .-minus
	.globl	operands
	.type	operands, @function
operands:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	w0, [x29, #24]
	ldr	w19, [x29, #24]
	add	w0, w19, #5
	str	w0, [x29, #28]
	adrp	x16, g
	ldr	w0, [x16, :lo12:g]
	add	w0, w19, w0
	str	w0, [x29, #32]
	ldr	w0, [x29, #28]
	ldr	w1, [x29, #32]
	add	w0, w0, w1
	b	.Loperands.1
.Loperands.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	operands, .-operands
	.globl	row
	.type	row, @function
row:
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp]
	.cfi_offset w29, -48
	.cfi_offset w30, -40
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x19, [x29, #16]
	.cfi_offset w19, -32
	str	x0, [x29, #24]
	str	w1, [x29, #32]
	ldr	x19, [x29, #24]
	ldr	w0, [x29, #32]
	str	w0, [x29, #36]
	ldr	w0, [x29, #36]
	bl	f
	sxtw	x0, w0
	mov	x1, #12
	mul	x0, x0, x1
	add	x0, x19, x0
	adrp	x16, h
	str	x0, [x16, :lo12:h]
.Lrow.1:
	.cfi_def_cfa wsp, 48
	ldr	x19, [x29, #16]
	.cfi_restore w19
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	row, .-row
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.globl	h
	.type	h, @object
	.size	h, 8
	.p2align 3
h:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
