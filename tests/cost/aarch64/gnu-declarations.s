	.file	"gnu-declarations.c"
	.text
	.globl	use
	.type	use, @function
use:
	.cfi_startproc
	sub	sp, sp, #96
	.cfi_def_cfa_offset 96
	stp	x29, x30, [sp]
	.cfi_offset w29, -96
	.cfi_offset w30, -88
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	str	x1, [x29, #24]
	str	x2, [x29, #32]
	ldr	x0, [x29, #16]
	str	x0, [x29, #40]
	ldr	x0, [x29, #40]
	bl	count
	str	x0, [x29, #48]
	ldr	x0, [x29, #24]
	ldr	w0, [x0]
	str	w0, [x29, #56]
	ldr	w0, [x29, #56]
	bl	other_name
	ldr	x1, [x29, #48]
	add	w1, w1, w0
	str	x1, [x29, #64]
	ldr	x0, [x29, #24]
	str	x0, [x29, #72]
	ldr	x0, [x29, #32]
	str	x0, [x29, #80]
	ldr	x0, [x29, #72]
	ldr	x1, [x29, #80]
	bl	add
	ldr	x1, [x29, #64]
	add	w0, w1, w0
	b	.Luse.1
.Luse.1:
	.cfi_def_cfa wsp, 96
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #96
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	use, .-use
	.globl	local
	.type	local, @function
local:
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
	str	w0, [x29, #20]
	ldr	w0, [x29, #20]
	add	w0, w0, #1
	b	.Llocal.1
.Llocal.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	local, .-local
	.bss
	.type	local.calls, @object
	.size	local.calls, 4
	.p2align 2
local.calls:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
