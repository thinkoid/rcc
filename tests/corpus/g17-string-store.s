	.file	"g17-string-store.c"
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
	movl	%edi, -4(%rbp)
	leaq	.Ls0(%rip), %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movb	$122, (%rax)
	movq	-16(%rbp), %rax
	movsbl	(%rax), %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.data
.Ls0:
	.string	"abc"
	.section	.note.GNU-stack,"",@progbits
