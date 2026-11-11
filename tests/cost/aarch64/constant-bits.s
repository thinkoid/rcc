	.file	"constant-bits.c"
	.text
	.globl	add_bits
	.type	add_bits, @function
add_bits:
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
	add	w0, w0, #4095
	add	w0, w0, #4096
	mov	w1, #5000
	add	w0, w0, w1
	mov	w1, #-5000
	add	w0, w0, w1
	mov	w1, #0x80000001
	add	w0, w0, w1
	mov	w1, #2
	movk	w1, #0x8000, lsl #16
	add	w0, w0, w1
	b	.Ladd_bits.1
.Ladd_bits.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	add_bits, .-add_bits
	.globl	and_bits
	.type	and_bits, @function
and_bits:
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
	and	w0, w0, #0xff
	mov	w1, #5
	and	w0, w0, w1
	b	.Land_bits.1
.Land_bits.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	and_bits, .-and_bits
	.globl	long_bits
	.type	long_bits, @function
long_bits:
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
	mov	x1, #-34464
	movk	x1, #0xfffe, lsl #16
	add	x0, x0, x1
	mov	x1, #281470681743360
	add	x0, x0, x1
	mov	x1, #0xffff0000ffff0000
	add	x0, x0, x1
	b	.Llong_bits.1
.Llong_bits.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	long_bits, .-long_bits
	.section	.note.GNU-stack,"",@progbits
