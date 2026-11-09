	.file	"g11-misaligned.c"
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
	movl	$0, -16(%rbp)
.Lmain.2:
	movl	-16(%rbp), %eax
	cmpl	$8, %eax
	jge	.Lmain.4
	leaq	-12(%rbp), %rax
	movl	-16(%rbp), %ecx
	movslq	%ecx, %rcx
	movl	-16(%rbp), %edx
	movl	-4(%rbp), %esi
	addl	%esi, %edx
	movb	%dl, (%rax,%rcx,1)
.Lmain.3:
	addl	$1, -16(%rbp)
	jmp	.Lmain.2
.Lmain.4:
	leaq	-12(%rbp), %rax
	addq	$1, %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	sarl	$24, %eax
	imull	$10, %eax, %eax
	movl	-20(%rbp), %ecx
	andl	$255, %ecx
	addl	%ecx, %eax
	jmp	.Lmain.1
.Lmain.1:
	leave
	.cfi_def_cfa %rsp, 8
	ret
	.cfi_endproc
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
