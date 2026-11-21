	.file	"anonymous-member.c"
	.text
	.globl	read_union
	.type	read_union, @function
read_union:
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
	ldr	x0, [x0, #8]
	b	.Lread_union.1
.Lread_union.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read_union, .-read_union
	.globl	write_union
	.type	write_union, @function
write_union:
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
	str	x1, [x0, #8]
.Lwrite_union.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	write_union, .-write_union
	.globl	read_struct
	.type	read_struct, @function
read_struct:
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
	ldrsh	w0, [x0, #16]
	ldr	x1, [x29, #16]
	ldr	w1, [x1, #20]
	add	w0, w0, w1
	b	.Lread_struct.1
.Lread_struct.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	read_struct, .-read_struct
	.globl	write_struct
	.type	write_struct, @function
write_struct:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	w1, [x29, #24]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	strh	w1, [x0, #16]
	ldr	x0, [x29, #16]
	ldr	w1, [x29, #24]
	str	w1, [x0, #20]
.Lwrite_struct.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	write_struct, .-write_struct
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
	mov	w0, #1
	str	w0, [x29, #24]
	mov	w0, #2
	str	w0, [x29, #36]
	ldr	w0, [x29, #24]
	ldr	w1, [x29, #36]
	add	w0, w0, w1
	b	.Llocal.1
.Llocal.1:
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	local, .-local
	.globl	global
	.type	global, @function
global:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	mov	x0, #3
	adrp	x16, g+8
	str	x0, [x16, :lo12:g+8]
	adrp	x16, g+8
	ldr	x0, [x16, :lo12:g+8]
	adrp	x16, g+16
	ldrsh	x1, [x16, :lo12:g+16]
	add	x0, x0, x1
	b	.Lglobal.1
.Lglobal.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	global, .-global
	.globl	both
	.type	both, @function
both:
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
	mov	w1, #1
	str	w1, [x0]
	ldr	x0, [x29, #16]
	ldr	w0, [x0, #4]
	b	.Lboth.1
.Lboth.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	both, .-both
	.globl	listed
	.type	listed, @function
listed:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	xzr, [x29, #16]
	mov	w0, #5
	str	w0, [x29, #16]
	mov	w0, #6
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	b	.Llisted.1
.Llisted.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	listed, .-listed
	.data
	.globl	preset
	.type	preset, @object
	.size	preset, 24
	.p2align 3
preset:
	.long	1
	.zero	4
	.long	2
	.zero	4
	.short	3
	.zero	2
	.long	4
	.bss
	.globl	g
	.type	g, @object
	.size	g, 24
	.p2align 3
g:
	.zero	24
	.section	.note.GNU-stack,"",@progbits
