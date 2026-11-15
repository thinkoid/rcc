	.file	"static.c"
	.text
	.type	hidden, @function
hidden:
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
	add	w0, w0, #1
	b	.Lhidden.1
.Lhidden.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	hidden, .-hidden
	.globl	shown
	.type	shown, @function
shown:
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
	add	w0, w0, #1
	b	.Lshown.1
.Lshown.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	shown, .-shown
	.globl	calls
	.type	calls, @function
calls:
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	stp	x29, x30, [sp]
	.cfi_offset w29, -64
	.cfi_offset w30, -56
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	w0, [x29, #16]
	adrp	x0, hidden
	add	x0, x0, :lo12:hidden
	str	x0, [x29, #24]
	adrp	x0, shown
	add	x0, x0, :lo12:shown
	str	x0, [x29, #32]
	ldr	w0, [x29, #16]
	str	w0, [x29, #40]
	ldr	w0, [x29, #40]
	bl	hidden
	str	x0, [x29, #48]
	ldr	w0, [x29, #16]
	str	w0, [x29, #56]
	ldr	w0, [x29, #56]
	bl	shown
	ldr	x1, [x29, #48]
	add	w1, w1, w0
	sxtw	x1, w1
	ldr	x0, [x29, #24]
	add	x1, x1, x0
	ldr	x0, [x29, #32]
	sub	x1, x1, x0
	mov	x0, x1
	b	.Lcalls.1
.Lcalls.1:
	.cfi_def_cfa wsp, 64
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	calls, .-calls
	.globl	get_quiet
	.type	get_quiet, @function
get_quiet:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, quiet
	ldr	w0, [x16, :lo12:quiet]
	b	.Lget_quiet.1
.Lget_quiet.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	get_quiet, .-get_quiet
	.globl	get_loud
	.type	get_loud, @function
get_loud:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, loud
	ldr	w0, [x16, :lo12:loud]
	b	.Lget_loud.1
.Lget_loud.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	get_loud, .-get_loud
	.globl	next
	.type	next, @function
next:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, next.count
	ldr	w0, [x16, :lo12:next.count]
	add	w0, w0, #1
	adrp	x16, next.count
	str	w0, [x16, :lo12:next.count]
	b	.Lnext.1
.Lnext.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	next, .-next
	.globl	next_total
	.type	next_total, @function
next_total:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, total
	ldr	w0, [x16, :lo12:total]
	add	w0, w0, #1
	adrp	x16, total
	str	w0, [x16, :lo12:total]
	b	.Lnext_total.1
.Lnext_total.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	next_total, .-next_total
	.bss
	.type	quiet, @object
	.size	quiet, 4
	.p2align 2
quiet:
	.zero	4
	.globl	loud
	.type	loud, @object
	.size	loud, 4
	.p2align 2
loud:
	.zero	4
	.globl	total
	.type	total, @object
	.size	total, 4
	.p2align 2
total:
	.zero	4
	.type	next.count, @object
	.size	next.count, 4
	.p2align 2
next.count:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
