	.file	"builtin-expect-volatile.c"
	.text
	.globl	expect_ready
	.type	expect_ready, @function
expect_ready:
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
	str	x0, [x29, #24]
	adrp	x16, ready
	ldr	w0, [x16, :lo12:ready]
	ldr	x0, [x29, #24]
	b	.Lexpect_ready.1
.Lexpect_ready.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	expect_ready, .-expect_ready
	.bss
	.globl	ready
	.type	ready, @object
	.size	ready, 4
	.p2align 2
ready:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
