	.file	"cast-marked-round-trip.c"
	.text
	.globl	through_void
	.type	through_void, @function
through_void:
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
	adrp	x16, plain
	str	x0, [x16, :lo12:plain]
	adrp	x16, plain
	ldr	x0, [x16, :lo12:plain]
	str	x0, [x29, #16]
.Lthrough_void.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	through_void, .-through_void
	.globl	through_integer
	.type	through_integer, @function
through_integer:
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
	adrp	x16, plain_result
	str	x0, [x16, :lo12:plain_result]
.Lthrough_integer.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	through_integer, .-through_integer
	.globl	same
	.type	same, @function
same:
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
	adrp	x16, marked
	str	x0, [x16, :lo12:marked]
.Lsame.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	same, .-same
	.bss
	.globl	plain
	.type	plain, @object
	.size	plain, 8
	.p2align 3
plain:
	.zero	8
	.globl	plain_result
	.type	plain_result, @object
	.size	plain_result, 8
	.p2align 3
plain_result:
	.zero	8
	.globl	marked
	.type	marked, @object
	.size	marked, 8
	.p2align 3
marked:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
