	.file	"global-array.c"
	.text
	.globl	get
	.type	get, @function
get:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, table
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #2
	ldr	w0, [x0, :lo12:table]
	b	.Lget.1
.Lget.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	get, .-get
	.globl	third
	.type	third, @function
third:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, table+12
	ldr	w0, [x16, :lo12:table+12]
	b	.Lthird.1
.Lthird.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	third, .-third
	.globl	put
	.type	put, @function
put:
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
	adrp	x0, table
	ldr	w1, [x29, #16]
	add	x0, x0, w1, sxtw #2
	ldr	w1, [x29, #20]
	str	w1, [x0, :lo12:table]
.Lput.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	put, .-put
	.globl	get_other
	.type	get_other, @function
get_other:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, :got:other
	ldr	x0, [x0, :got_lo12:other]
	ldr	w1, [x29, #16]
	ldr	w0, [x0, w1, sxtw #2]
	b	.Lget_other.1
.Lget_other.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	get_other, .-get_other
	.globl	third_other
	.type	third_other, @function
third_other:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, :got:other
	ldr	x0, [x0, :got_lo12:other]
	ldr	w0, [x0, #12]
	b	.Lthird_other.1
.Lthird_other.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	third_other, .-third_other
	.bss
	.globl	table
	.type	table, @object
	.size	table, 32
	.p2align 2
table:
	.zero	32
	.section	.note.GNU-stack,"",@progbits
