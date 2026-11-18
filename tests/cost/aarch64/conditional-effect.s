	.file	"conditional-effect.c"
	.text
	.globl	pick
	.type	pick, @function
pick:
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
	cbz	w0, .Lpick.2
	bl	f
	b	.Lpick.3
.Lpick.2:
	bl	g
.Lpick.3:
.Lpick.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pick, .-pick
	.globl	dropped
	.type	dropped, @function
dropped:
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
	cbz	w0, .Ldropped.2
	bl	h
	b	.Ldropped.3
.Ldropped.2:
	bl	k
.Ldropped.3:
.Ldropped.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	dropped, .-dropped
	.globl	nested
	.type	nested, @function
nested:
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
	ldr	w0, [x29, #16]
	cbz	w0, .Lnested.2
	ldr	w0, [x29, #20]
	cbz	w0, .Lnested.4
	bl	f
	b	.Lnested.5
.Lnested.4:
	bl	g
.Lnested.5:
	b	.Lnested.3
.Lnested.2:
	bl	f
.Lnested.3:
.Lnested.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	nested, .-nested
	.globl	stored
	.type	stored, @function
stored:
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
	cbz	w0, .Lstored.2
	mov	w0, #1
	adrp	x16, x
	str	w0, [x16, :lo12:x]
	b	.Lstored.3
.Lstored.2:
	mov	w0, #2
	adrp	x16, x
	str	w0, [x16, :lo12:x]
.Lstored.3:
.Lstored.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stored, .-stored
	.bss
	.globl	x
	.type	x, @object
	.size	x, 4
	.p2align 2
x:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
