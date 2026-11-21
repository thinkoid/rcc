	.file	"conditional-omitted-operand.c"
	.text
	.globl	in_memory
	.type	in_memory, @function
in_memory:
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
	cbnz	w0, .Lin_memory.2
	ldr	w0, [x29, #20]
.Lin_memory.2:
	adrp	x16, h
	str	w0, [x16, :lo12:h]
.Lin_memory.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	in_memory, .-in_memory
	.globl	called
	.type	called, @function
called:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	f
	cbnz	w0, .Lcalled.2
	bl	g
.Lcalled.2:
	b	.Lcalled.1
.Lcalled.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	called, .-called
	.globl	pointer
	.type	pointer, @function
pointer:
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
	cbnz	x0, .Lpointer.2
	ldr	x0, [x29, #24]
.Lpointer.2:
	b	.Lpointer.1
.Lpointer.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	pointer, .-pointer
	.globl	narrow
	.type	narrow, @function
narrow:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	strb	w0, [x29, #16]
	str	w1, [x29, #20]
	ldrb	w0, [x29, #16]
	cbnz	w0, .Lnarrow.2
	ldr	w0, [x29, #20]
.Lnarrow.2:
	b	.Lnarrow.1
.Lnarrow.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	narrow, .-narrow
	.globl	condition
	.type	condition, @function
condition:
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
	cbnz	w0, .Lcondition.3
	ldr	w0, [x29, #20]
	cbz	w0, .Lcondition.2
.Lcondition.3:
	mov	w0, #1
	b	.Lcondition.1
.Lcondition.2:
	mov	w0, #0
	b	.Lcondition.1
.Lcondition.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	condition, .-condition
	.globl	into_register
	.type	into_register, @function
into_register:
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
	str	w0, [x29, #24]
	str	w1, [x29, #28]
	ldr	w19, [x29, #24]
	cbnz	w19, .Linto_register.2
	ldr	w19, [x29, #28]
.Linto_register.2:
	mov	w0, w19
	b	.Linto_register.1
.Linto_register.1:
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
	.size	into_register, .-into_register
	.globl	returned
	.type	returned, @function
returned:
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
	cbnz	w0, .Lreturned.2
	ldr	w0, [x29, #20]
.Lreturned.2:
	b	.Lreturned.1
.Lreturned.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	returned, .-returned
	.globl	discarded
	.type	discarded, @function
discarded:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	bl	f
	cbnz	w0, .Ldiscarded.2
	bl	g
.Ldiscarded.2:
.Ldiscarded.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	discarded, .-discarded
	.bss
	.globl	h
	.type	h, @object
	.size	h, 4
	.p2align 2
h:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
