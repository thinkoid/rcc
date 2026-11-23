	.file	"static-inline-c1-not-yet.c"
	.text
	.globl	main
	.type	main, @function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	.Ls0(%rip), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movsbl	(%rax), %eax
	subl	$109, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"main"
	.section	.note.GNU-stack,"",@progbits
