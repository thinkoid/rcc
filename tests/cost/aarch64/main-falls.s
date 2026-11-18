	.file	"main-falls.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	stp	x29, x30, [sp]
	.cfi_offset w29, -16
	.cfi_offset w30, -8
	mov	x29, sp
	.cfi_def_cfa_register w29
	adrp	x16, g
	ldr	w0, [x16, :lo12:g]
	cbz	w0, .Lmain.2
	mov	w0, #1
	b	.Lmain.1
.Lmain.2:
	mov	w0, #0
.Lmain.1:
	.cfi_def_cfa wsp, 16
	ldp	x29, x30, [sp]
	.cfi_restore w29
	.cfi_restore w30
	add	sp, sp, #16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
	.size	main, .-main
	.bss
	.globl	g
	.type	g, @object
	.size	g, 4
	.p2align 2
g:
	.zero	4
	.section	.note.GNU-stack,"",@progbits
