	.file	"switch-simple.c"
	.text
	.globl	kind
	.type	kind, @function
kind:
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
	cmp	w0, #-1
	b.eq	.Lkind.4
	cbz	w0, .Lkind.3
	cmp	w0, #10
	b.eq	.Lkind.2
	cmp	w0, #100
	b.eq	.Lkind.5
	b	.Lkind.6
.Lkind.2:
	mov	w0, #1
	b	.Lkind.1
.Lkind.3:
	mov	w0, #2
	b	.Lkind.1
.Lkind.4:
	mov	w0, #3
	b	.Lkind.1
.Lkind.5:
	mov	w0, #4
	b	.Lkind.1
.Lkind.6:
	mov	w0, #5
	b	.Lkind.1
.Lkind.7:
.Lkind.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	kind, .-kind
	.section	.note.GNU-stack,"",@progbits
