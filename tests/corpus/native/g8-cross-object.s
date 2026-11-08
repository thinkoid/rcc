	.file	"g8-cross-object.c"
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
	movl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	$2, %eax
	movl	%eax, -12(%rbp)
	leaq	-12(%rbp), %rax
	leaq	-8(%rbp), %rcx
	subq	%rcx, %rax
	movq	%rax, -24(%rbp)
	leaq	-8(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	-24(%rbp), %rcx
	addq	%rcx, %rax
	movl	$3, %ecx
	movl	%ecx, (%rax)
	movl	-12(%rbp), %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
