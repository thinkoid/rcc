	.file	"null-pointer-constant-qualified.c"
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
	mov	w0, #0
	cbz	w0, .Lmain.2
	mov	w0, #101
	b	.Lmain.1
.Lmain.2:
	mov	w0, #0
	cbz	w0, .Lmain.3
	mov	w0, #102
	b	.Lmain.1
.Lmain.3:
	mov	w0, #0
	cbz	w0, .Lmain.4
	mov	w0, #103
	b	.Lmain.1
.Lmain.4:
	mov	w0, #42
	b	.Lmain.1
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
	.globl	c
	.type	c, @object
	.size	c, 4
	.p2align 2
c:
	.zero	4
	.globl	ip
	.type	ip, @object
	.size	ip, 8
	.p2align 3
ip:
	.zero	8
	.section	.note.GNU-stack,"",@progbits
