	.file	"wide-string.c"
	.text
	.globl	greeting
	.type	greeting, @function
greeting:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	b	.Lgreeting.1
.Lgreeting.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	greeting, .-greeting
	.globl	joined
	.type	joined, @function
joined:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	b	.Ljoined.1
.Ljoined.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	joined, .-joined
	.globl	wide_constants
	.type	wide_constants, @function
wide_constants:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	x0, #63329
	movk	x0, #0x1, lsl #16
	b	.Lwide_constants.1
.Lwide_constants.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	wide_constants, .-wide_constants
	.globl	second
	.type	second, @function
second:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, .Ls2+4
	ldr	w0, [x16, :lo12:.Ls2+4]
	b	.Lsecond.1
.Lsecond.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	second, .-second
	.data
	.p2align 2
.Ls0:
	.long	104, 233, 0
	.p2align 2
.Ls1:
	.long	233, 33, 0
	.p2align 2
.Ls2:
	.long	120, 121, 122, 0
	.data
	.globl	padded
	.type	padded, @object
	.size	padded, 32
	.p2align 2
padded:
	.long	97, 98, 0
	.zero	20
	.globl	code_units
	.type	code_units, @object
	.size	code_units, 8
	.p2align 1
code_units:
	.short	120, 55357, 56832, 0
	.section	.note.GNU-stack,"",@progbits
