	.file	"struct-object.c"
	.text
	.globl	locals
	.type	locals, @function
locals:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	add	x0, x29, #16
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	use
	add	x0, x29, #20
	str	x0, [x29, #48]
	ldr	x0, [x29, #48]
	bl	use
	add	x0, x29, #32
	str	x0, [x29, #56]
	ldr	x0, [x29, #56]
	bl	use
	ldr	w0, [x29, #28]
	b	.Llocals.1
.Llocals.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	locals, .-locals
	.globl	globals
	.type	globals, @function
globals:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, three
	add	x0, x0, :lo12:three
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	bl	use
	adrp	x0, one
	add	x0, x0, :lo12:one
	str	x0, [x29, #24]
	ldr	x0, [x29, #24]
	bl	use
	adrp	x16, wide+8
	ldr	w0, [x16, :lo12:wide+8]
	adrp	x16, six+4
	ldrsh	w1, [x16, :lo12:six+4]
	add	w0, w0, w1
	b	.Lglobals.1
.Lglobals.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	globals, .-globals
	.globl	element
	.type	element, @function
element:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, pairs+40
	ldr	x0, [x16, :lo12:pairs+40]
	b	.Lelement.1
.Lelement.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	element, .-element
	.globl	sizes
	.type	sizes, @function
sizes:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	x0, #77
	b	.Lsizes.1
.Lsizes.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	sizes, .-sizes
	.bss
	.globl	three
	.type	three, @object
	.size	three, 3
three:
	.zero	3
	.globl	six
	.type	six, @object
	.size	six, 6
	.p2align 1
six:
	.zero	6
	.globl	wide
	.type	wide, @object
	.size	wide, 12
	.p2align 2
wide:
	.zero	12
	.globl	pairs
	.type	pairs, @object
	.size	pairs, 48
	.p2align 3
pairs:
	.zero	48
	.type	one, @object
	.size	one, 8
	.p2align 3
one:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
