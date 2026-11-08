	.file	"g1-add-compare.c"
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
	movl	$2147483646, %eax
	movl	-4(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	-8(%rbp), %ecx
	cmpl	%ecx, %eax
	setg	%al
	movzbl	%al, %eax
	jmp	.L1
.L1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
