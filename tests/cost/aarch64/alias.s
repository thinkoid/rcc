	.file	"alias.c"
	.text
	.globl	fgcmd
	.type	fgcmd, @function
fgcmd:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	w0, #0
	b	.Lfgcmd.1
.Lfgcmd.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	fgcmd, .-fgcmd
	.globl	statically
	.type	statically, @function
statically:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	fgalias
	b	.Lstatically.1
.Lstatically.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	statically, .-statically
	.globl	statically_counted
	.type	statically_counted, @function
statically_counted:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, :got:salias
	ldr	x0, [x0, :got_lo12:salias]
	ldr	w0, [x0]
	b	.Lstatically_counted.1
.Lstatically_counted.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	statically_counted, .-statically_counted
	.data
	.globl	count
	.type	count, @object
	.size	count, 4
	.p2align 2
count:
	.long	1
	.globl	bgcmd
	.type	bgcmd, @function
bgcmd = fgcmd
	.globl	total
total = count
	.type	fgalias, @function
fgalias = fgcmd
	.weak	wbgcmd
	.type	wbgcmd, @function
wbgcmd = fgcmd
	.weak	wtotal
wtotal = count
salias = count
	.section	.note.GNU-stack,"",@progbits
