	.file	"static-inline-not-yet.c"
	.text
	.type	one, @function
one:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, %eax
	jmp	.Lone.1
.Lone.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	one, .-one
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	call	one@PLT
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
