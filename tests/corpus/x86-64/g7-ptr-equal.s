	.file	"g7-ptr-equal.c"
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
	leaq	-8(%rbp), %rax
	addq	$4, %rax
	leaq	-12(%rbp), %rcx
	cmpq	%rcx, %rax
	sete	%al
	movzbl	%al, %eax
	leaq	-12(%rbp), %rcx
	addq	$4, %rcx
	leaq	-8(%rbp), %rdx
	cmpq	%rdx, %rcx
	sete	%cl
	movzbl	%cl, %ecx
	addl	%ecx, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
