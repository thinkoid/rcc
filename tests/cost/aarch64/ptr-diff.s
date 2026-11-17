	.file	"ptr-diff.c"
	.text
	.globl	diff
	.type	diff, @function
diff:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	x1, [x29, #24]
	sub	x0, x0, x1
	mov	x1, #4
	sdiv	x0, x0, x1
	b	.Ldiff.1
.Ldiff.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	diff, .-diff
	.globl	register_char
	.type	register_char, @function
register_char:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	adrp	x0, chars
	add	x0, x0, :lo12:chars
	sub	x0, x19, x0
	b	.Lregister_char.1
.Lregister_char.1:
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
	.size	register_char, .-register_char
	.globl	register_int
	.type	register_int, @function
register_int:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	adrp	x0, ints
	add	x0, x0, :lo12:ints
	sub	x0, x19, x0
	mov	x1, #4
	sdiv	x0, x0, x1
	b	.Lregister_int.1
.Lregister_int.1:
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
	.size	register_int, .-register_int
	.globl	register_row
	.type	register_row, @function
register_row:
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
	str	x0, [x29, #24]
	ldr	x19, [x29, #24]
	adrp	x0, rows
	add	x0, x0, :lo12:rows
	sub	x0, x19, x0
	mov	x1, #12
	sdiv	x0, x0, x1
	b	.Lregister_row.1
.Lregister_row.1:
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
	.size	register_row, .-register_row
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
	str	x19, [x29, #16]
	.cfi_offset w19, -16
	adrp	x19, ints+4
	add	x19, x19, :lo12:ints+4
	adrp	x0, ints
	add	x0, x0, :lo12:ints
	sub	x0, x19, x0
	mov	x1, #4
	sdiv	x0, x0, x1
	b	.Lassigned.1
.Lassigned.1:
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
	.size	assigned, .-assigned
	.bss
	.globl	chars
	.type	chars, @object
	.size	chars, 8
chars:
	.zero	8
	.globl	ints
	.type	ints, @object
	.size	ints, 32
	.p2align 2
ints:
	.zero	32
	.globl	rows
	.type	rows, @object
	.size	rows, 96
	.p2align 2
rows:
	.zero	96
	.section	.note.GNU-stack,"",@progbits
