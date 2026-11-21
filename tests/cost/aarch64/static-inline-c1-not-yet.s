	.file	"static-inline-c1-not-yet.c"
	.text
	.type	stride, @function
stride:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	str	x0, [x29, #16]
	adrp	x0, .Ls0
	add	x0, x0, :lo12:.Ls0
	str	x0, [x29, #24]
	adrp	x16, stride.calls
	ldr	w0, [x16, :lo12:stride.calls]
	add	w0, w0, #1
	adrp	x16, stride.calls
	str	w0, [x16, :lo12:stride.calls]
	ldr	x0, [x29, #16]
	mov	x1, #2147483648
	ldrb	w0, [x0, x1]
	ldr	x1, [x29, #24]
	ldrb	w1, [x1]
	add	w0, w0, w1
	b	.Lstride.1
.Lstride.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	stride, .-stride
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	stp	x29, x30, [sp]
	.cfi_offset w29, -32
	.cfi_offset w30, -24
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x0, .Ls1
	add	x0, x0, :lo12:.Ls1
	str	x0, [x29, #16]
	ldr	x0, [x29, #16]
	ldrb	w0, [x0]
	sub	w0, w0, #109
	b	.Lmain.1
.Lmain.1:
	.cfi_def_cfa wsp, 32
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #32
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"stride"
.Ls1:
	.string	"main"
	.bss
	.type	stride.calls, @object
	.size	stride.calls, 4
	.p2align 2
stride.calls:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
